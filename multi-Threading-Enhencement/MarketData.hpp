//
//  MarketData.hpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 5/1/22.
//
#pragma once
#include <string>
#include "Stock.hpp"
#include "sqlite3.h"
#include "Database.hpp"
#include <vector>
class Stock; // forward declaration

int PullMarketData(const std::string& url_request, std::string& read_buffer);
int PopulateDailyTrades(const std::string &read_buffer, Stock& stock);

void PopulatePairs(vector<StockPairPrices>& all_Pairs, const string& file_name);
int PopulateStocks(const vector<StockPairPrices>& all_Pairs,sqlite3 * &db, string t_start, string t_end);

// below are for multi-threads
int FetchData(map<string,Stock> & stocks, string start_date, string end_date);
int PopulatePairPrices_tables(sqlite3 * &db, const map<string,Stock> & stocks, bool flg_firstpair);
