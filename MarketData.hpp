//
//  MarketData.hpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 5/1/22.
//
#pragma once
#include <string>

class Stock; // forward declaration

int PullMarketData(const std::string& url_request, std::string& read_buffer);
int PopulateDailyTrades(const std::string &read_buffer, Stock& stock);

