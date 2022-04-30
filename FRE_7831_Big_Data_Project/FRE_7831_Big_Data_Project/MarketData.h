//
//  MarketData.h
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Elaine Huang on 2022/4/10.
//  Copyright © 2022 NYU. All rights reserved.
//


#include <string>
#include <stdio.h>
//#include "Stock.hpp"
#include "Util.hpp"
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
 // forward declaration

int PullMarketData(const std::string& url_request, std::string& read_buffer);

int GetPairsVec(const char *infile, std::vector<std::pair<std::string,std::string>> &PairVec);
