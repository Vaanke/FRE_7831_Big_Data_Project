//
//  Util.hpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#pragma once
//#include "Stock.hpp"
#include <vector>
#include "json/json.h"
#include "MarketData.hpp"
#include "Database.hpp"
#include "sqlite3.h"
//using namespace std;



map<string, string> ProcessConfigData(string config_file);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
int Insert_intoPairPrices(sqlite3* & db);

int Calculate_Vol(sqlite3* & db, string back_test_start_date);
float Calc_pnl_manual(float sigma, int k, float open1d2, float close1d1,float close1d2, float open2d2, float close2d1, float close2d2);
int Calc_PnL(sqlite3 * db, float k_mutiplier);
int Calc_PnL_cumulative(sqlite3 * db);

bool isNums(const string str);
bool isNumber(string ss);
