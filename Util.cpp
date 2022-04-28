//
//  Util.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#include "Util.hpp"

// read the PairTrading.txt and populate stockPair Prices
void PopulateParis(vector<StockPairPrices>& all_Pairs, const string& file_name) {}
void PopulateStocks(map<string, Stock>& all_Stocks, const vector<StockPairPrices>& all_Pairs){}
void PopulatePairPrices(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs){}
void Calc_Vol(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs){}
void Calc_PnL(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs){}
void Create_PairTable(const vector<StockPairPrices>& all_Pairs){}
void Create_PairPricesTable(const vector<StockPairPrices>& all_Pairs){}
