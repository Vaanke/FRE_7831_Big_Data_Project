#pragma once
//#include "Stock.hpp"
#include <vector>
#include "json/json.h"
#include "MarketData.h"
#include "Database.hpp"
//using namespace std;



void Calc_PnL(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs); // update PnL in StockPairPrices using the existing vol and stock data

// Process config file for Market Data Retrieval
map<string, string> ProcessConfigData(string config_file);

//writing call back function for storing fetched values in memory
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

//calculate pnl for manual testing
float Calc_pnl_manual(float sigma, int k, float open1d2, float close1d1, float close1d2, float open2d2, float close2d1, float close2d2);
