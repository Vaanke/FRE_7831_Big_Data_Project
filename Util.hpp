//
//  Util.hpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#pragma once
#include "Stock.hpp"
#include <vector>
#include "json/json.h"

using namespace std;



void PopulateParis(vector<StockPairPrices>& all_Pairs, const string& file_name); // read the PairTrading.txt and populate stockPair Prices
void PopulateStocks(map<string, Stock>& all_Stocks, const vector<StockPairPrices>& all_Pairs); // use info from vec of stock paris to get stock tickers, and fetch data from web thro tickers
void PopulatePairPrices(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs); // transfer daily data from all_stocks to stockPairPrices
void Calc_Vol(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs); // update vol in StockPairPrices using the existing stock data. Vol = ratio of two adj_close price for each pair, from 1/1/2012 - 12/31/2021
void Calc_PnL(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs); // update PnL in StockPairPrices using the existing vol and stock data

void Create_PairTable(const vector<StockPairPrices>& all_Pairs);
void Create_PairPricesTable(const vector<StockPairPrices>& all_Pairs);
