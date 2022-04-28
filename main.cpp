//
//  main.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//

#include <iostream>
#include "Stock.hpp"
#include <string>
#include <map>
#include <vector>
#include "Util.hpp"
using namespace std;



int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    string path_in = "PairTrading.txt";
    vector<StockPairPrices> all_Pairs;
    map<string, Stock> all_Stocks;
    
    
    PopulateParis(all_Pairs, path_in); // read the PairTrading.txt and populate stockPair Prices
    PopulateStocks(all_Stocks, all_Pairs); // use info from vec of stock paris to get stock tickers, and fetch data from web thro tickers
    PopulatePairPrices(all_Stocks, all_Pairs); // transfer daily data from all_stocks to stockPairPrices

    
    bool bCompleted = false;
    char option;
       // Menu
    while (!bCompleted) {
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
       
        cin >> option;
       switch (option) {
           case 'A':{
               // Create and Populate Pair Table, not populated with vol yet
               cout << "Completed!" << endl;
               break;
           }
               
           case 'B':{
               // Retrieve and Populate Historical Data for Each Stock
               cout << "Completed!" << endl;
               break;
           }
               
           case 'C': {
               // Create PairPrices Table
               // Create all three pair prices table.
               cout << "Completed!" << endl;
               break;
           }
           case 'D': {
               // Calculate Violatility
               // populate stockpairs table with vol
               cout << "Completed!" << endl;
               break;
           }
           case 'E': {
               // Back Test, 1/1/2022 - now
               
               cout << "Completed!" << endl;
               break;
           }
           case 'F': {
               // Calculate Profit and Loss for Each Pair
               cout << "Completed!" << endl;
               break;
           }
           case 'G': {
               cout << "Completed!" << endl;
               break;
           }
           case 'H': {
               cout << "Completed!" << endl;
               break;
               }
           case 'X': {
               bCompleted = true;
               break;
               }
       }
    
    return 0;
}
    
    
    
}

