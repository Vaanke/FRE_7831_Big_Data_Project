#pragma once
#include "Stock.hpp"
#include <vector>
#include "json/json.h"
#include "MarketData.h"
#include "Database.hpp"
using namespace std;


void PopulatePairs(vector<StockPairPrices>& all_Pairs, const string& file_name); // read the PairTrading.txt and populate stockPair Prices

void PopulateStocks(map<string, Stock>& all_Stocks, const vector<StockPairPrices>& all_Pairs); // use info from vec of stock paris to get stock tickers, and fetch data from web thro tickers
void PopulatePairPrices(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs); // transfer daily data from all_stocks to stockPairPrices
void Calc_Vol(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs); // update vol in StockPairPrices using the existing stock data. Vol = ratio of two adj_close price for each pair, from 1/1/2012 - 12/31/2021
void Calc_PnL(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs); // update PnL in StockPairPrices using the existing vol and stock data


int Create_PairTable(const vector<StockPairPrices>& all_Pairs,sqlite3* db);
void Create_PairPricesTable(const vector<StockPairPrices>& all_Pairs);
// Process config file for Market Data Retrieval
map<string, string> ProcessConfigData(string config_file);

//writing call back function for storing fetched values in memory
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

void Create_PairTable(const vector<StockPairPrices>& all_Pairs);
