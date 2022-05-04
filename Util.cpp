//
//  Util.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//

#include "Util.hpp"
#include <unordered_set>
#include <iostream>
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
        getline(sin, stock2,',');
        pair<string, string> stockpair;
        
        if (!stock2.empty() && stock2[stock2.length() - 1] == '\r') stock2.erase(stock2.size() - 1);
        
        stockpair.first=stock1;
        stockpair.second=stock2;
        all_Pairs.push_back(StockPairPrices(stockpair));
        
    }
}



int PopulateStocks(const vector<StockPairPrices>& all_Pairs,sqlite3 * &db, string t_start, string t_end)
{
    
    const string start_date = t_start;
    const string end_date = t_end;
    const string daily_url_common = "https://eodhistoricaldata.com/api/eod/";
    const string api_token = "623fdfffcf36b8.23759772";
    
    
    unordered_set<string> PairOneStocks;
    unordered_set<string> PairTwoStocks;
    
    
    for (int i =0; i<(int)all_Pairs.size(); i++) {
        pair<string, string> stockpair;
        
        stockpair=all_Pairs[i].GetStockPair();
        string StockOne = stockpair.first.c_str();
        string StockTwo = stockpair.second.c_str();
        
        
        // Stock One
        if (PairOneStocks.find(StockOne) == PairOneStocks.end()) {
            string url_request = daily_url_common + StockOne + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
            string read_buffer_StockOne;
            vector<TradeData> atrade_;
            Stock StockFirst(StockOne, atrade_);

            if (PullMarketData(url_request, read_buffer_StockOne)!=0) return -1;
            if (PopulateDailyTrades(read_buffer_StockOne, StockFirst) != 0)
            {
                cerr << "ERROR: failed to Populate Daily Trades for" << StockFirst.getSymbol() << std::endl;
                CloseDatabase(db);
                return -1;
            }
            // Add the stock into the set
            // threading: segement stocks tickers into groups, multi access web for downloading data, then combine data and insert to table.
            PairOneStocks.insert(StockOne);
            if (InsertTable(db, true, StockFirst) != 0)
            {
                cerr << "ERROR: Failed to insert data into PairOnePrices" << endl;
            }
            
            cout << StockFirst.getSymbol()<<": Successfully inserted into PairOnePrices Table" << endl;
            
            
        }
        
        // Stock Two
        if (PairTwoStocks.find(StockTwo) == PairTwoStocks.end())
        {
            string url_request = daily_url_common + StockTwo + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
            
            string read_buffer_StockTwo;
            vector<TradeData> atrade_;
            Stock StockSecond(StockTwo, atrade_);
            
            if (PullMarketData(url_request, read_buffer_StockTwo)!=0) return -1;
            
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
            cout << StockSecond.getSymbol()<<": Successfully inserted into PairTwoPrices Table" << endl;
        }
        
        cout<< "Pair of " << StockOne << " & " << StockTwo << " is done.\n" << endl;
        
    }
    return 0;
    
}



int Calculate_Vol(sqlite3* & db, string back_test_start_date){

    cout << "Populating Volatility in StockPrices table ..." << endl;
    
    string calculate_volatility_for_pair = string("Update StockPairs SET volatility =")
    + "(SELECT(AVG((adjusted_close1/adjusted_close2)*(adjusted_close1/ adjusted_close2)) - AVG(adjusted_close1/adjusted_close2)*AVG(adjusted_close1/adjusted_close2)) as volatility "
    + "FROM PairPrices "
    + "WHERE StockPairs.symbol1 = PairPrices.symbol1 AND StockPairs.symbol2 = PairPrices.symbol2 AND PairPrices.date <= \'"
    + back_test_start_date + "\');";
    
    if( ExecuteSQL(db, calculate_volatility_for_pair.c_str()) !=0) return -1;
    
    return 0;
}


float Calc_pnl_manual(float sigma, int k, float open1d2, float close1d1, float close1d2, float open2d2, float close2d1, float close2d2){
    bool Short;
    float profit;
    int N1 = 10000;
    int N2 = N1 * (open1d2 / open2d2);

    if (abs(close1d1 / close2d1 - open1d2 / open2d2) > k * sigma) {
        Short = true;
    } else {
        Short = false;
    }

    if (Short) {
        profit = N1 * (open1d2 - close1d2) - N2 * (open2d2 - close2d2);
        cout << "- Action: Short" << endl;
    } else {
        profit = -N1 * (open1d2 - close1d2) + N2 * (open2d2 - close2d2);
        cout << "- Action: Long" << endl;
    }
    return profit;
}



int Calc_PnL(sqlite3 * db,  float k_mutiplier){
    
    string sql_stmt_Calc_PnL = string("UPDATE PairPrices ")
    +"set profit_loss = ( "
    +"    SELECT all_pnl.calc_pnl "
    +"    From( "
    +"        select *, "
    +"            CASE when diff > volatility * " + to_string(k_mutiplier) + "  and date > '2022-01-03' then - pnl_long "
    +"                 when diff < volatility * " + to_string(k_mutiplier) + "  and date > '2022-01-03' then pnl_long "
    +"                 else 0 "
    +"            END    as calc_pnl "
    +"        from( "
    +"            select *,"
    +"                abs(lag((close1 / close2),1) over (PARTITION BY symbol1, symbol2 order by date) -  open1 / open2 ) as diff, "
    +"                10000 * (close1 - open1) - 10000 * (open1 / open2) * (close2 - open2) as pnl_long "
    +"            from ( "
    +"                select * from StockPairs sp join PairPrices pp "
    +"                where sp.symbol1 = pp.symbol1 and sp.symbol2 = pp.symbol2 "
    +"                ) "
    +"            as     all_data "
    +"            ) "
    +"        ) "
    +"    as all_pnl "
    +"    WHERE "
    +"        all_pnl.symbol1 = PairPrices.symbol1 and all_pnl.symbol2 = PairPrices.symbol2 and all_pnl.date = PairPrices.date "
    +"); " ;
//    ExecuteSQL(db, sql_stmt_Calc_PnL.c_str());
    if ( ExecuteSQL(db, sql_stmt_Calc_PnL.c_str()) == -1 ) {
        cout << "ERROR: Failed to execute sql Calc_PNL" << endl;
        return -1;
    }
    return 0;
}




int Calc_PnL_cumulative(sqlite3 * db){
    
    string sql_stmt_Calc_cumPnL = string("UPDATE StockPairs ")
    +"set profit_loss = ( "
    +"    SELECT sumPNL from "
    +"        (SELECT symbol1, symbol2, sum(profit_loss) as sumPNL from PairPrices pp group by symbol1, symbol2) "
    +"        as cum_pnl "
    +"    WHERE StockPairs.symbol1 = cum_pnl.symbol1 and StockPairs.symbol2 = cum_pnl.symbol2 "
    +"    ); " ;
    
    if ( ExecuteSQL(db, sql_stmt_Calc_cumPnL.c_str()) == -1 ) {
        cout << "ERROR: Failed to execute sql Calc_cumulative_PNL" << endl;
        return -1;
    }
    return 0;
}



int Create_PairTable(const vector<StockPairPrices>& all_Pairs,sqlite3* db){
    
//    cout << "Drop Pair table if exists" << endl;
    string sql_DropaTable = "DROP TABLE IF EXISTS StockPairs";
    if (DropTable(db, sql_DropaTable.c_str()) == -1)
        return -1;
    
//    cout << "Create tables" << endl;
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
    
    cout << "Finished Creating PairOnePrices & PairTwoPrices\n " << endl;
    
    return 0;
}

int Create_PairPricesTable(sqlite3* & db){
    
    string sql_DropTable = string("DROP TABLE IF EXISTS PairPrices");
    if(DropTable(db,sql_DropTable.c_str()) !=0) return -1;
    
    cout << "Creating PairPrices table ..." << endl;
    string sql_CreateTable = string("CREATE TABLE IF NOT EXISTS PairPrices ")
    +"(symbol1 CHAR(20) NOT NULL,"
    +"symbol2 CHAR(20) NOT NULL,"
    +"date CHAR(20) NOT NULL,"
    +"open1 REAL NOT NULL,"
    +"close1 REAL NOT NULL,"
    +"adjusted_close1 REAL NOT NULL,"
    +"open2 REAL NOT NULL,"
    +"close2 REAL NOT NULL,"
    +"adjusted_close2 REAL NOT NULL,"
    +"profit_loss REAL NOT NULL,"
    +"PRIMARY KEY(symbol1, symbol2, date),"
    +"Foreign Key(symbol1, date) references PairOnePrices(symbol, date)\n"
    +"Foreign Key(symbol2, date) references PairTwoPrices(symbol, date)\n"
    +"Foreign Key(symbol1, symbol2) references StockPairs(symbol1, symbol2)\n"
    +"ON DELETE CASCADE\n"
    +"ON UPDATE CASCADE);";
    
    if( ExecuteSQL(db, sql_CreateTable.c_str()) !=0) return -1;
    cout<<"PairPrices table created."<<endl;
    
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


int Insert_intoPairPrices(sqlite3* & db){
    cout << "Populating PairPrices table ..." << endl;
    string sql_InsertTable = string("Insert into PairPrices ")
    +"Select StockPairs.symbol1 as symbol1, StockPairs.symbol2 as symbol2,"
    +"PairOnePrices.date as date,"
    +"PairOnePrices.open as open1, PairOnePrices.close as close1, PairOnePrices.adjusted_close as adjusted_close1,"
    +"PairTwoPrices.open as open2, PairTwoPrices.close as close2, PairTwoPrices.adjusted_close as adjusted_close2,"
    +"0 as profit_loss "
    +"From StockPairs, PairOnePrices, PairTwoPrices "
    +"Where (((StockPairs.symbol1 = PairOnePrices.symbol) and "
    +"(StockPairs.symbol2 = PairTwoPrices.symbol)) and "
    +"(PairOnePrices.date = PairTwoPrices.date)) "
    +"ORDER BY symbol1, symbol2;";
    
    if( ExecuteSQL(db, sql_InsertTable.c_str()) !=0) return -1;
    cout << "PairPrices table populated ." << endl;
    return 0;
}



bool isNums(const string str)
{
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

bool isNumber(string ss){
    
    if (ss[0] == '-'){
        ss.erase(0,1); // erase singn
    }
    if (ss.find_first_of(".") != string::npos) {
        size_t posi = (int)ss.find_first_of(".");
        ss.erase(posi, 1);
        return isNums(ss);
    }
    else {
        return isNums(ss);
    }
}
