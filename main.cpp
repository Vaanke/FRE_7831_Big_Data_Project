//
//  main.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#include <iostream>
#include "Database.hpp"
#include "MarketData.hpp"
#include <string>
#include <map>
#include <vector>
#include "Util.hpp"
#include <chrono>
#include <unistd.h>
using namespace std;

int main() {
    
    string path_in = "PairTrading.txt";
    vector<StockPairPrices> all_Pairs;
    string database_name = "pair_trading.db";
    cout << "Opening Database..." << endl;
    sqlite3* db = NULL;
    
    if (OpenDatabase(database_name.c_str(), db) != 0)  return -1;
    
    string t_start = "2012-01-01"; // start from 2012-01-01
    string t_end = "2022-04-30";
    char selection;
    do
    {
        std::cout << endl;
        std::cout << "Menu" << endl;
        std::cout << "========" << endl;
        std::cout << "A - Create and Populate Pair Table" << endl;
        std::cout << "B - Retrieve and Populate Historical Data for Each Stock" << endl;
        std::cout << "C - Create PairPrices Table" << endl;
        std::cout << "D - Calculate Violatility" << endl;
        std::cout << "E - Back Test" << endl;
        std::cout << "F - Calculate Profit and Loss for Each Pair" << endl;
        std::cout << "G - Manual Testing" << endl;
        std::cout << "H - Drop All the Tables" << endl << endl;
        std::cout << "X - Exit" << endl << endl;
        
        std::cout <<"Enter selection: ";
        std::cin >> selection;
        switch (selection) {
            case 'A':
            case 'a':
            {
                // Create and Populate Pair Table, not populated with vol yet
                PopulatePairs(all_Pairs, path_in); // read the PairTrading.txt and populate stockPair Prices
                for (int i =0; i<(int)all_Pairs.size(); i++){
                    cout<< all_Pairs[i].GetStockPair().first<< " "<<all_Pairs[i].GetStockPair().second<<endl;
                }
                if (Create_PairTable(all_Pairs, db) == -1){
                    return -1;
                }
                cout <<endl << "A Completed!" << endl;
                break;
            }
            case 'B':
            case 'b':
            {
                // Retrieve and Populate Historical Data for Each Stock
                auto start = chrono::steady_clock::now();
                if (Create_PairOneTwoPricesTable(db) != 0) {return -1;}
                if (PopulateStocks(all_Pairs, db, t_start, t_end)!=0) {return -1;}
                
                auto end = chrono::steady_clock::now();
                cout    << "Elapsed time in seconds: "
                        << chrono::duration_cast<chrono::seconds>(end - start).count()
                        << " sec" << endl;
                cout <<endl << "B Completed!" << endl;
                break;
            }
                
            case 'C':
            case 'c':
            {
                // Create PairPrices Table
                // Create all three pair prices table.
                
                if(Create_PairPricesTable(db)!=0) return -1;
                if(Insert_intoPairPrices(db)!=0) return -1;
                cout << endl << "C Completed!" << endl;
                break;
            }
            case 'D':
            case 'd':
            {
                // Calculate Violatility
                string back_test_start_date_ = "2021-12-31";
                if(Calculate_Vol(db, back_test_start_date_)!=0) return -1;
                cout << endl << "D Completed!" << endl;
                break;
            }
            case 'E':
            case 'e':
            {
                // Back Test, 1/1/2022 - now
                float k_mutiple;
                cout << "Please enter a volatility mutiplier: ";
                cin >> k_mutiple;
                if (Calc_PnL(db, k_mutiple) != 0) return -1;
                cout << endl << "E Completed!" << endl;
                break;
            }
            case 'F':
            case 'f':
            {
                // Calculate Profit and Loss for Each Pair
                if (Calc_PnL_cumulative(db) != 0) return -1;
                cout << endl << "F Completed!" << endl;
                break;
            }
            case 'G':
            case 'g':
            {
                string symbol1, symbol2, test_date;
                float open1d2, close1d2, open2d2, close2d2;
                float k;
                cout << endl;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter Symbol1: ";
                cin >> symbol1;
                cout << "Enter Open Price for " + symbol1 +": " ;
                cin >> open1d2;
                cout << "Enter Close Price for " + symbol1 +": " ;
                cin >> close1d2;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter Symbol2: ";
                cin >> symbol2;
                cout << "Enter Open Price for " + symbol2 +": " ;
                cin >> open2d2;
                cout << "Enter Close Price for " + symbol2 +": ";
                cin >> close2d2;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter k: ";
                cin >> k;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter previous trade date for testing purpose in format of xxxx-xx-xx: " ;
                cin >> test_date;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << " " << endl;
                cout << " " << endl;
                
                string sql_get_vol = string("SELECT volatility FROM StockPairs ")
                +"WHERE StockPairs.symbol1 = '"
                +symbol1
                +"' AND StockPairs.symbol2 = '"
                +symbol2+"';";
                
                char **results_vol = NULL;
                int rows_vol, columns_vol;
                char *error_vol = nullptr;
                sqlite3_get_table(db, sql_get_vol.c_str(), &results_vol, &rows_vol, &columns_vol, &error_vol);
                if (error_vol) {
                    cout << error_vol;
                }
                float sigma = stof(results_vol[1]);
                
                
                cout << "TESTING SUMMARY" << endl;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout<< "- Vol for ( " + symbol1 + " , " + symbol2 + " ) is: " <<sigma<<endl;
                
                string sql_get_price = string("SELECT open1, close1, open2, close2 FROM PairPrices ")
                +"WHERE PairPrices.symbol1 = '"+symbol1+"' AND PairPrices.symbol2 = '"+symbol2+"' AND PairPrices.date = \'"
                + test_date + "\';";
                
                char **results_price = NULL;
                int rows_price, columns_price;
                char *error_price = nullptr;
                sqlite3_get_table(db, sql_get_price.c_str(), &results_price, &rows_price, &columns_price, &error_price);
                if (error_price) {
                    cout << error_price;
                }
                
                float close1d1 = stof(results_price[5]);
                float close2d1 = stof(results_price[7]);
                
                float pnl = Calc_pnl_manual(sigma,k,open1d2,close1d1,close1d2,open2d2,close2d1,close2d2);
                
                
                //float price = stof(results_price[0]);
                cout<<"- PnL for pair "+symbol1+" and "+symbol2+" is: "<<pnl<<endl << endl;
                cout << endl << "G Completed!" << endl;
                break;
            }
            case 'H':
            case 'h':
            {
                string sql_dropPOP = " DROP TABLE IF EXISTS PairOnePrices ";;
                string sql_dropPTP = "DROP TABLE IF EXISTS PairTwoPrices;";
                string sql_dropPP = "DROP TABLE IF EXISTS PairPrices;";
                string sql_dropSP = "DROP TABLE IF EXISTS StockPairs;";
                string sql_foreign_off = " PRAGMA foreign_keys=off;";
                string sql_foreign_dropon = " PRAGMA foreign_keys=on; ";
                
                if (ExecuteSQL(db, sql_foreign_off.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "Foreign key check is off ..." << endl;
                }
                if (DropTable(db, sql_dropPOP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "PairOnePrices table is dropped" << endl;
                }
                if (DropTable(db, sql_dropPTP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "PairTwoPrices table is dropped" << endl;
                }
                if (DropTable(db, sql_dropPP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "PairPrices table is dropped" << endl;
                }
                if (DropTable(db, sql_dropSP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "StockPairs table is dropped" << endl;
                }
                if (ExecuteSQL(db, sql_foreign_dropon.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "Foreign key check is on !" << endl;
                }

                cout << endl << "H Completed!" << endl;
                break;
            }
            case 'X':
            case 'x':
            {
                break;
            }
            default:
            {
                cerr << "Invalid selection. Choose again." << endl;
                break;
            }
        }
        cout << endl;
        
    } while (selection != 'X' and selection != 'x');
    
    
    CloseDatabase(db);
    
    return 0;
}
