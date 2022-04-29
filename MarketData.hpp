#pragma once
#include <string>
#include "Util.hpp"
class Stock; // forward declaration

int PullMarketData(const std::string& url_request, std::string& read_buffer);

int PopulateDailyTrades(const std::string& read_buffer,Stock& stock);
