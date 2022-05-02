//
//  Database.cpp
//  lab_marketData
//
//  Created by Deyang Van on 4/10/22.
//

#include "Database.hpp"
#include <iostream>
#include <string>
#include "json/json.h"
#include <math.h>
#include <sqlite3.h>

int OpenDatabase(const char* database_name, sqlite3* & db)
{
    int rc = 0;
    rc = sqlite3_open(database_name, &db);
    if (rc)
    {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }
    else
    {
        std::cout << "Opened " << database_name << std::endl;
    }

    std::string foreign_ley_check = "PRAGMA foreign_keys = ON";
    if (sqlite3_exec(db, foreign_ley_check.c_str(), 0, 0, 0) != SQLITE_OK) {
        std::cout << "SQLite fails in setting foreign key check" << std::endl;
        sqlite3_close(db);
        return -1;
    }

    return 0;
}

int DropTable(sqlite3 * db, const char* sql_stmt)
{
    if (sqlite3_exec(db, sql_stmt, 0, 0, 0) != SQLITE_OK) {
        std::cout << "SQLite can not drop table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    return 0;
}

int ExecuteSQL(sqlite3* db, const char* sql_stmt)
{
    int rc = 0;
    char* error = nullptr;
    rc = sqlite3_exec(db, sql_stmt, NULL, NULL, &error);
    if (rc)
    {
        std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_free(error);
        return -1;
    }
    return 0;
}

int ShowTable(sqlite3* db, const char* sql_stmt)
{
    int rc = 0;
    char* error = nullptr;
    char** results = NULL;
    int rows, columns;
    sqlite3_get_table(db, sql_stmt, &results, &rows, &columns, &error);
    if (rc)
    {
        std::cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << std::endl << std::endl;
        sqlite3_free(error);
        return -1;
    }
    else
    {
        // Display Table
        for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                // Determine Cell Position
                int cellPosition = (rowCtr * columns) + colCtr;

                // Display Cell Value
                std::cout.width(12);
                std::cout.setf(std::ios::left);
                std::cout << results[cellPosition] << " ";
            }

            // End Line
            std::cout << std::endl;

            // Display Separator For Header
            if (0 == rowCtr)
            {
                for (int colCtr = 0; colCtr < columns; ++colCtr)
                {
                    std::cout.width(12);
                    std::cout.setf(std::ios::left);
                    std::cout << "~~~~~~~~~~~~ ";
                }
                std::cout << std::endl;
            }
        }
    }
    sqlite3_free_table(results);
    return 0;
}

int InsertTable(sqlite3* db, bool IsStockOne, const Stock &stock)
{
    string date;
    double open,high,low,close,adjusted_close;
    long volume;
    
    vector<TradeData> atrade = stock.getTrades();

    // insert data into PairOnePrice
    for(vector<TradeData>::iterator itr = atrade.begin(); itr != atrade.end(); itr ++)
    {

        date = itr->GetsDate();
        open = itr->GetdOpen();
        high = itr->GetdHigh();
        low = itr->GetdLow();
        close = itr->GetdClose();
        adjusted_close = itr->GetdAdjClose();
        volume = itr->GetlVolumn();
        
        char sqlInsert[512];

        if (IsStockOne)
        {
        //cout << "Inserting a value into table PairOnePrices" << endl;
        sprintf(sqlInsert, "INSERT INTO PairOnePrices(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %ld)", stock.getSymbol().c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
        } else
        {
        //cout << "Inserting a value into table PairTwoPrices" << endl;
        sprintf(sqlInsert, "INSERT INTO PairTwoPrices(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %ld)", stock.getSymbol().c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
        }
        if(ExecuteSQL(db, sqlInsert) !=0) return -1;
    }
    return 0;
}


void CloseDatabase(sqlite3* db)
{
    sqlite3_close(db);
}

string sql_stmt_insert_Daily(string symbol, string date, float open, float high, float low, float close, float adjusted_close, int volume){
    // "INSERT INTO PERSON VALUES(1, 'STEVE', 'GATES', 30, 'PALO ALTO', 1000.0);"
    string sql_stmt = string("INSERT INTO ")
        + "DailyTrades "
        + "VALUES("
        + "'"+ symbol + "'"+ ","
        + "'"+ date + "'"+ ","
        + to_string(open) + ","
        + to_string(high) + ","
        + to_string(low) + ","
        + to_string(close) + ","
        + to_string(adjusted_close) + ","
        + to_string(volume)
        + ");";
    
    return sql_stmt;
}

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

int Calculate_Vol(sqlite3* & db, string back_test_start_date){
//    sqlite3_create_function(db, "sqrt_sql",1,SQLITE_UTF8,nullptr,sqrt_sql,nullptr,nullptr);
    cout << "Populating Volatility in StockPrices table ..." << endl;
    string calculate_volatility_for_pair = string("Update StockPairs SET volatility =")
    + "(SELECT(AVG((adjusted_close1/adjusted_close2)*(adjusted_close1/ adjusted_close2)) - AVG(adjusted_close1/adjusted_close2)*AVG(adjusted_close1/adjusted_close2)) as volatility "
    + "FROM PairPrices "
    + "WHERE StockPairs.symbol1 = PairPrices.symbol1 AND StockPairs.symbol2 = PairPrices.symbol2 AND PairPrices.date <= \'"
    + back_test_start_date + "\');";
    
    if( ExecuteSQL(db, calculate_volatility_for_pair.c_str()) !=0) return -1;
    return 0;
}
