//
//  Util.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#pragma once
#include "Util.hpp"
#include <unordered_set>
#include <ctime>
#include <cstdio>


// read the PairTrading.txt and populate stockPair Prices
void Calc_PnL(const map<string, Stock>& all_Stocks, vector<StockPairPrices>& all_Pairs){}


map<string, string> ProcessConfigData(string config_file)
{
    map<string, string> config_map;
    ifstream fin;
    fin.open(config_file, ios::in);
    string line, name, value;
    while (!fin.eof())
    {
        getline(fin, line);
        stringstream sin(line);
        getline(sin, name, ':');
        getline(sin, value, '\r');
        //        cout << value << endl;
        //        cout << value.back() << endl;
        
        config_map.insert(pair<string, string>(name, value));
    }
    return config_map;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

float Calc_pnl_manual(float sigma, int k, float open1d2, float close1d1, float close1d2, float open2d2, float close2d1, float close2d2){
    bool Short;
    float profit;
    int N1 = 10000;
    int N2 = N1 * (open1d2 / open2d2);

    if (abs(close1d1 / close2d1 - open1d2 / open2d2) > k * sigma) {
        Short = true;
    } else {
        Short = false;
    }

    if (Short) {
        profit = N1 * (open1d2 - close1d2) - N2 * (open2d2 - close2d2);
    } else {
        profit = -N1 * (open1d2 - close1d2) + N2 * (open2d2 - close2d2);
    }
    return profit;
}
