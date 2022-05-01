//
//  Util.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#pragma once
#include "Util.hpp"
#include <unordered_set>
#include <ctime>
#include <cstdio>


// read the PairTrading.txt and populate stockPair Prices
void PopulatePairs(vector<StockPairPrices>& all_Pairs, const string& file_name) {
    string line, stock1, stock2;
    ifstream fin;
    fin.open(file_name,ios::in);
    
    //populate pair
    
    while (getline(fin,line)){
        stringstream sin(line);
        getline(sin, stock1, ',');
        getline(sin, stock2);
        pair<string, string> stockpair;
        stockpair.first=stock1;
        stockpair.second=stock2;
        all_Pairs.push_back(StockPairPrices(stockpair));
        
        
    }
}
void PopulatePairPrices(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs){}
void Calc_Vol(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs){}
void Calc_PnL(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs){}

int Create_PairTable(const vector<StockPairPrices>& all_Pairs,sqlite3* db){
    
    cout << "Drop Pair table if exists" << endl;
    string sql_DropaTable = "DROP TABLE IF EXISTS StockPairs";
    if (DropTable(db, sql_DropaTable.c_str()) == -1)
        return -1;
    
    cout << "Create tables" << endl;
    string sql_CreateTable = string("CREATE TABLE IF NOT EXISTS StockPairs ")
    + "(id INT NOT NULL,"
    + "symbol1 CHAR(20) NOT NULL,"
    + "symbol2 CHAR(20) NOT NULL,"
    + "volatility REAL NOT NULL,"
    + "profit_loss REAL NOT NULL,"
    + "PRIMARY KEY(symbol1,symbol2)"
    + ");";
    if (ExecuteSQL(db, sql_CreateTable.c_str()) == -1)
        return -1;

    cout << "Created 1 table" << endl;
    
    
    char sql_Insert[512];
    
    for (int i =0; i<(int)all_Pairs.size(); i++) {
        pair<string, string> stockpair;
        stockpair=all_Pairs[i].GetStockPair();
        sprintf(sql_Insert, "INSERT INTO StockPairs(id, symbol1, symbol2,volatility,profit_loss) VALUES(%d, \"%s\", \"%s\", %f, %f)", i+1, stockpair.first.c_str(), stockpair.second.c_str(), 0.0,0.0);
          if (ExecuteSQL(db, sql_Insert) == -1)
              return -1;
    }
    return 0;
}


int Create_PairOneTwoPricesTable(sqlite3 * &db)
{
    string PairOnePrices_sql_create_table = string("CREATE TABLE IF NOT EXISTS PairOnePrices ")
        + "(symbol CHAR(20) NOT NULL,"
        + "date CHAR(20) NOT NULL,"
        + "open REAL NOT NULL,"
        + "high REAL NOT NULL,"
        + "low REAL NOT NULL,"
        + "close REAL NOT NULL,"
        + "adjusted_close REAL NOT NULL,"
        + "volume INT NOT NULL,"
        + "PRIMARY KEY(symbol, date));";
    
    string PairTwoPrices_sql_create_table = string("CREATE TABLE IF NOT EXISTS PairTwoPrices ")
        + "(symbol CHAR(20) NOT NULL,"
        + "date CHAR(20) NOT NULL,"
        + "open REAL NOT NULL,"
        + "high REAL NOT NULL,"
        + "low REAL NOT NULL,"
        + "close REAL NOT NULL,"
        + "adjusted_close REAL NOT NULL,"
        + "volume INT NOT NULL,"
        + "PRIMARY KEY(symbol, date));";
    
    // Drop the tables if they already exist in the database
    string sql_droptable1 = "DROP TABLE IF EXISTS PairOnePrices";
    if(DropTable(db,sql_droptable1.c_str()) !=0) return -1;

    string sql_droptable2 = "DROP TABLE IF EXISTS PairTwoPrices";
    if(DropTable(db,sql_droptable2.c_str()) !=0) return -1;

    // Create tables
    if(ExecuteSQL(db, PairOnePrices_sql_create_table.c_str()) !=0) return -1;
    if(ExecuteSQL(db, PairTwoPrices_sql_create_table.c_str()) !=0) return -1;
    
    cout << "Finished Creating PairOnePrices & PairTwoPrices " << endl;

    return 0;
}

int PopulateStocks(const vector<StockPairPrices>& all_Pairs,sqlite3 * &db)
{
    
    string start_date = "2012-01-01";
    string end_date = "2022-04-30";
    string daily_url_common = "https://eodhistoricaldata.com/api/eod/";
    string api_token = "623fdfffcf36b8.23759772";
    
    
    unordered_set<string> PairOneStocks;
    unordered_set<string> PairTwoStocks;

    
    for (int i =0; i<(int)all_Pairs.size(); i++) {
        pair<string, string> stockpair;
        
        stockpair=all_Pairs[i].GetStockPair();
        string StockOne = stockpair.first.c_str();
        string StockTwo = stockpair.second.c_str();
        cout<<StockOne;

        string daily_url_request_StockOne = daily_url_common + StockOne + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
        string daily_url_request_StockTwo = daily_url_common + StockTwo + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
        
//        string read_buffer_StockOne;
//        string read_buffer_StockTwo;

        // Stock One
        if (PairOneStocks.find(StockOne) == PairOneStocks.end())
        {
            string url_request = daily_url_common + StockOne + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
            string read_buffer_StockOne;
            vector<TradeData> atrade_;
            Stock StockFirst(StockOne, atrade_);

            if (PullMarketData(daily_url_request_StockOne, read_buffer_StockOne)!=0) return -1;

            if (PopulateDailyTrades(read_buffer_StockOne, StockFirst) != 0)
            {
                cerr << "ERROR: failed to Populate Daily Trades for" << StockFirst.getSymbol() << std::endl;
                CloseDatabase(db);
                return -1;
            }
            
            // Add the stock into the set
            PairOneStocks.insert(StockOne);
            if (InsertTable(db, true, StockFirst) != 0)
            {
                cerr << "ERROR: Failed to insert data into PairOnePrices" << endl;
            }
            
        }
        
        // Stock Two
        if (PairTwoStocks.find(StockTwo) == PairTwoStocks.end())
        {
            string url_request = daily_url_common + StockTwo + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";

            cout<<url_request;

            string read_buffer_StockTwo;
            vector<TradeData> atrade_;
            Stock StockSecond(StockTwo, atrade_);

            if (PullMarketData(daily_url_request_StockTwo, read_buffer_StockTwo)!=0) return -1;

            if (PopulateDailyTrades(read_buffer_StockTwo, StockSecond) != 0)
            {
                cerr << "ERROR: failed to Populate Daily Trades for" << StockSecond.getSymbol() << std::endl;
                CloseDatabase(db);
                return -1;
            }
            
            // Add the stock into the set
            PairTwoStocks.insert(StockTwo);
            
            if (InsertTable(db, false, StockSecond) != 0)
            {
                cerr << "ERROR: Failed to insert data into PairTwoPrices" << endl;
            }
        }

    }
    return 0;
    
}


map<string, string> ProcessConfigData(string config_file)
{
    map<string, string> config_map;
    ifstream fin;
    fin.open(config_file, ios::in);
    string line, name, value;
    while (!fin.eof())
    {
        getline(fin, line);
        stringstream sin(line);
        getline(sin, name, ':');
        getline(sin, value, '\r');
//        cout << value << endl;
//        cout << value.back() << endl;
        
        config_map.insert(pair<string, string>(name, value));
    }
    return config_map;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
