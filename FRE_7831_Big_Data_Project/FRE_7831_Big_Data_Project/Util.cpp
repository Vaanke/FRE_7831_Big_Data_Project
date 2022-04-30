//
//  Util.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#pragma once
#include "Util.hpp"

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
void PopulateStocks(map<string, Stock>& all_Stocks, const vector<StockPairPrices>& all_Pairs){}
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
void Create_PairPricesTable(const vector<StockPairPrices>& all_Pairs){}

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
