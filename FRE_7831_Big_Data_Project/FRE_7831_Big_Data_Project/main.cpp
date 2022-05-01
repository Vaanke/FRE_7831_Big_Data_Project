#include <iostream>
//#include "Stock.hpp"
#include "Database.hpp"
#include "MarketData.h"

#include <string>
#include <map>
#include <vector>
#include "Util.hpp"
using namespace std;



int main(int argc, const char * argv[]) {
    
    string path_in = "/Users/elaine_huang/Desktop/NYUmfe/2022_Spring/FRE_7831/Final_Project/FRE_7831_Big_Data_Project/FRE_7831_Big_Data_Project/DerivedData/FRE_7831_Big_Data_Project/Build/Products/Debug/PairTrading.txt";
    vector<StockPairPrices> all_Pairs;
    //map<string, Stock> all_Stocks;
    
    
    
    //PopulateStocks(all_Stocks, all_Pairs); // use info from vec of stock paris to get stock tickers, and fetch data from web thro tickers
    //PopulatePairPrices(all_Stocks, all_Pairs); // transfer daily data from all_stocks to stockPairPrices
    
    
    //open database
    string database_name = "/Users/elaine_huang/Desktop/NYUmfe/2022_Spring/FRE_7831/Final_Project/FRE_7831_Big_Data_Project/FRE_7831_Big_Data_Project/DerivedData/FRE_7831_Big_Data_Project/Build/Products/Debug/pair_trading.db";
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
                cout << "Completed!" << endl;
                break;
            }
            case 'D':
            case 'd':
            {
                // Calculate Violatility
                // populate stockpairs table with vol
                cout << "Completed!" << endl;
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
                cout << "Completed!" << endl;
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

