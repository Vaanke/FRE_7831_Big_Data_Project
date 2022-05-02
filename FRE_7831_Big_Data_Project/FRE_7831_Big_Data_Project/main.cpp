#include <iostream>
//#include "Stock.hpp"
#include "Database.hpp"
#include "MarketData.h"

#include <string>
#include <map>
#include <vector>
#include "Util.hpp"
#include <sqlite3.h>
using namespace std;



int main(int argc, const char * argv[]) {
    
    string path_in = "/Users/evyzheng/Desktop/FRE_7831_Big_Data_Project/DerivedData/FRE_7831_Big_Data_Project/Build/Products/Debug/PairTrading.txt";
    vector<StockPairPrices> all_Pairs;
    //map<string, Stock> all_Stocks;
    
    
    
    //PopulateStocks(all_Stocks, all_Pairs); // use info from vec of stock paris to get stock tickers, and fetch data from web thro tickers
    //PopulatePairPrices(all_Stocks, all_Pairs); // transfer daily data from all_stocks to stockPairPrices
    
    
    //open database
    string database_name = "/Users/evyzheng/Desktop/FRE_7831_Big_Data_Project/DerivedData/FRE_7831_Big_Data_Project/Build/Products/Debug/pair_trading.db";
    cout << "Opening Database..." << endl;
    sqlite3* db = NULL;
    if (OpenDatabase(database_name.c_str(), db) != 0)      return -1;
    
    
    //bool bCompleted = false;
    char selection;
    // Menu
    
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
                cout << "A Completed!" << endl;
                break;
            }
            case 'B':
            case 'b':
            {
                // Retrieve and Populate Historical Data for Each Stock
                if (Create_PairOneTwoPricesTable(db) != 0) {return -1;}
                if (PopulateStocks(all_Pairs, db)!=0) {return -1;}
                cout << "B Completed!" << endl;
                break;
            }
                
            case 'C':
            case 'c':
            {
                // Create PairPrices Table
                // Create all three pair prices table.
                if(Create_PairPricesTable(db)!=0) return -1;
                if(Insert_intoPairPrices(db)!=0) return -1;
                cout << "C Completed!" << endl;
                break;
            }
            case 'D':
            case 'd':
            {
                // Calculate Violatility
                // populate stockpairs table with vol
                string back_test_start_date_ = "2021-12-31";
                if(Calculate_Vol(db, back_test_start_date_)!=0) return -1;
                cout << "D Completed!" << endl;
                break;
            }
            case 'E':
            case 'e':
            {
                // Back Test, 1/1/2022 - now
                
                cout << "Completed!" << endl;
                break;
            }
            case 'F':
            case 'f':
            {
                // Calculate Profit and Loss for Each Pair
                cout << "Completed!" << endl;
                break;
            }
            case 'G':
            case 'g':
            {
                string symbol1, symbol2, test_date;
                float open1d2, close1d2, open2d2, close2d2;
                int k;
                
                cout << "Enter Symbol1" << endl;
                cin >> symbol1;
                cout << "Enter Open Price for " + symbol1 +":" << endl;
                cin >> open1d2;
                cout << "Enter Close Price for " + symbol1 +":"  << endl;
                cin >> close1d2;
                
                cout << "Enter Symbol2" << endl;
                cin >> symbol2;
                cout << "Enter Open Price for " + symbol2 +":" << endl;
                cin >> open2d2;
                cout << "Enter Close Price for " + symbol2 +":" << endl;
                cin >> close2d2;
                
                cout << "Enter k:" << endl;
                cin >> k;
                cout << "Enter date for testing in format xxxx-xx-xx:" <<endl;
                cin >> test_date;
                
                string sql_get_vol = string("SELECT volatility FROM StockPairs ")
                +"WHERE StockPairs.symbol1 = '"+symbol1+"' AND StockPairs.symbol2 = '"+symbol2+"';";
                
                char **results_vol = NULL;
                int rows_vol, columns_vol;
                char *error_vol = nullptr;
                sqlite3_get_table(db, sql_get_vol.c_str(), &results_vol, &rows_vol, &columns_vol, &error_vol);
                if (error_vol) {
                    cout << error_vol;
                }
                float sigma = stof(results_vol[1]);
                cout<<sigma<<endl;
                
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
                cout<<"PnL for pair "+symbol1+" and "+symbol2+" is: "<<pnl<<endl;
                
                cout << "G Completed!" << endl;
                break;
            }
            case 'H':
            case 'h':
            {
                cout << "Completed!" << endl;
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
        
    } while (selection != 'X');
    CloseDatabase(db);
    return 0;
}

