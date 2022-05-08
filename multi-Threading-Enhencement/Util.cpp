//
//  Util.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//

#include "Util.hpp"

#include <iostream>
#include <ctime>
#include <cstdio>




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
