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

class Stock; // forward declaration

int PullMarketData(const std::string& url_request, std::string& read_buffer);
int PopulateDailyTrades(const std::string &read_buffer, Stock& stock);

#endif /* MARKETDATA_H */
