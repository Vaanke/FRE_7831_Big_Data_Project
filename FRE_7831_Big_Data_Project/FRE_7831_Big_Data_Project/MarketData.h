//
//  MarketData.h
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Elaine Huang on 2022/4/10.
//  Copyright © 2022 NYU. All rights reserved.
//
#ifndef MARKETDATA_H
#define MARKETDATA_H

#include <string>
#include <vector>
#include "json/json.h"
#include "Database.hpp"

class Stock; // forward declaration

int PullMarketData(const std::string& url_request, std::string& read_buffer);

int PopulateDailyTrades(const std::string &read_buffer, Stock& stock);

void PopulatePairs(vector<StockPairPrices>& all_Pairs, const string& file_name); // read the PairTrading.txt and populate stockPair Prices

int PopulateStocks(const vector<StockPairPrices>& all_Pairs,sqlite3 * &db); // pull data using curl and populate stock12Prices



#endif /* MARKETDATA_H */
