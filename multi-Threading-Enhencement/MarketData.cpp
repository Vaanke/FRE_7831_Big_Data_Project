//
//  MarketData.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 5/1/22.
//


#include "MarketData.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include "Util.hpp"
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include <unordered_set>

int PullMarketData(const std::string& url_request, std::string& read_buffer)
{
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);

    //creating session handle
    CURL * handle;
    
    // Store the result of CURLís webpage retrieval, for simple error checking.
    CURLcode result;

    // notice the lack of major error-checking, for brevity
    handle = curl_easy_init();
    
    //fprintf(stderr, "curl_easy_init() failed: %s\n", handle);
    if (!handle)
    {
        std::cout << "curl_easy_init failed" << std::endl;
        return -1;
    }
    
    //after creating handle we will start transfering webpage
    //curl_easy_setopt is used to tell libcurl how to behave.
    //By setting the appropriate options, the application can change libcurl's behavior.

    curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
    //adding a user agent
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
    // send all data to this function
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    // we pass our 'chunk' struct to the callback function
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &read_buffer);

    //perform a blocking file transfer
    result = curl_easy_perform(handle);

    // check for errors
    if (result != CURLE_OK) {
        std::cout << "curl_easy_perform failed" << std::endl;
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        return -1;
    }
    
//    cout << read_buffer << endl;
    curl_easy_cleanup(handle);
    curl_global_cleanup();
    
    return 0;
}



int PopulateDailyTrades(const std::string &read_buffer, Stock& stock)
{
    // get daily trade data for evey stock
    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();
    Json::Value root;
    std::string errors;
    
    bool parsingSuccessful = reader->parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), &root, &errors);
    
    if (!parsingSuccessful)
    {
        std::cout << " Failed to parse JSON " << std::endl << read_buffer << ": " << errors << std::endl;
        return -1;
    }
    
    std::cout << stock.getSymbol()<<": Successfully parsing JSON" << std::endl;
    
    std::string date;
    double open;
    double high;
    double low;
    double close;
    double adjclose;
    long volume;
    
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        date = (*itr)["date"].asString();
        open = (*itr)["open"].asFloat();
        high = (*itr)["high"].asFloat();
        low = (*itr)["low"].asFloat();
        close = (*itr)["close"].asFloat();
        adjclose = (*itr)["adjusted_close"].asFloat();
        volume = (*itr)["volume"].asInt64();
        
        TradeData aTrade(date, open, high, low, close, adjclose, volume);
        stock.addTrade(aTrade);
    }
    return 0;
}





void PopulatePairs(vector<StockPairPrices>& all_Pairs, const string& file_name) {
    string line, stock1, stock2;
    ifstream fin;
    fin.open(file_name,ios::in);
    //populate pair
    
    while (getline(fin,line)){
        stringstream sin(line);
        getline(sin, stock1, ',');
        getline(sin, stock2,',');
        pair<string, string> stockpair;
        
        if (!stock2.empty() && stock2[stock2.length() - 1] == '\r') stock2.erase(stock2.size() - 1);
        
        stockpair.first=stock1;
        stockpair.second=stock2;
        all_Pairs.push_back(StockPairPrices(stockpair));
        
    }
}


int FetchData(map<string,Stock> & stocks, string start_date, string end_date) {
    // populate the stock map
    const string daily_url_common = "https://eodhistoricaldata.com/api/eod/";
    const string api_token = "623fdfffcf36b8.23759772";
    
    for (map<string, Stock>::iterator itr = stocks.begin(); itr != stocks.end(); itr++) {
        string symbol = itr->first;
        string url_request = daily_url_common + symbol + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
        string read_buffer;

        if (PullMarketData(url_request, read_buffer)!=0) return -1;
        if (PopulateDailyTrades(read_buffer, itr->second) != 0)
        {
            cerr << "ERROR: failed to Populate Daily Trades for" << itr->first << std::endl;
            return -1;
        }
    }
    return 0;
}



int PopulatePairPrices_tables(sqlite3 * &db, const map<string,Stock> & stocks, bool flg_firstpair) {
    // stocks should be the whole info, not partly one that used for thread
    for ( map<string, Stock>::const_iterator itr = stocks.begin(); itr != stocks.end(); itr++)
    {
        if (flg_firstpair) {
            if (InsertTable(db, true, itr->second) != 0)
            {
                cerr << "ERROR: Failed to insert" + itr->first + "data into PairOnePrices Tbale" << endl;
            } else {
                cout << itr->first <<" : Successfully inserted" << endl;
            }
        }
        else {
            if (InsertTable(db, false, itr->second) != 0)
            {
                cerr << "ERROR: Failed to insert" + itr->first + "data into PairTwoPrices Table" << endl;
            } else {
                cout << itr->first <<" : Successfully inserted" << endl;
            }
        }
        
    }
    return 0;
}



int PopulateStocks(const vector<StockPairPrices>& all_Pairs,sqlite3 * &db, string t_start, string t_end)
{
    
    const string start_date = t_start;
    const string end_date = t_end;
    const string daily_url_common = "https://eodhistoricaldata.com/api/eod/";
    const string api_token = "623fdfffcf36b8.23759772";
    
    
    unordered_set<string> PairOneStocks;
    unordered_set<string> PairTwoStocks;
    
    
    for (int i =0; i<(int)all_Pairs.size(); i++) {
        pair<string, string> stockpair;
        
        stockpair=all_Pairs[i].GetStockPair();
        string StockOne = stockpair.first.c_str();
        string StockTwo = stockpair.second.c_str();
        
        
        // Stock One
        if (PairOneStocks.find(StockOne) == PairOneStocks.end()) {
            string url_request = daily_url_common + StockOne + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
            string read_buffer_StockOne;
            vector<TradeData> atrade_;
            Stock StockFirst(StockOne, atrade_);

            if (PullMarketData(url_request, read_buffer_StockOne)!=0) return -1;
            if (PopulateDailyTrades(read_buffer_StockOne, StockFirst) != 0)
            {
                cerr << "ERROR: failed to Populate Daily Trades for" << StockFirst.getSymbol() << std::endl;
                CloseDatabase(db);
                return -1;
            }
            // Add the stock into the set
            // threading: segement stocks tickers into groups, multi access web for downloading data, then combine data and insert to table.
            PairOneStocks.insert(StockOne);
            if (InsertTable(db, true, StockFirst) != 0)
            {
                cerr << "ERROR: Failed to insert data into PairOnePrices" << endl;
            }
            
            cout << StockFirst.getSymbol()<<": Successfully inserted into PairOnePrices Table" << endl;
            
            
        }
        
        // Stock Two
        if (PairTwoStocks.find(StockTwo) == PairTwoStocks.end())
        {
            string url_request = daily_url_common + StockTwo + ".US?from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
            
            string read_buffer_StockTwo;
            vector<TradeData> atrade_;
            Stock StockSecond(StockTwo, atrade_);
            
            if (PullMarketData(url_request, read_buffer_StockTwo)!=0) return -1;
            
            if (PopulateDailyTrades(read_buffer_StockTwo, StockSecond) != 0)
            {
                cerr << "ERROR: failed to Populate Daily Trades for" << StockSecond.getSymbol() << std::endl;
                CloseDatabase(db);
                return -1;
            }
            
            // Add the stock into the set
            PairTwoStocks.insert(StockTwo);
            
            if (InsertTable(db, false, StockSecond) != 0)
            {
                cerr << "ERROR: Failed to insert data into PairTwoPrices" << endl;
            }
            cout << StockSecond.getSymbol()<<": Successfully inserted into PairTwoPrices Table" << endl;
        }
        
        cout<< "Pair of " << StockOne << " & " << StockTwo << " is done.\n" << endl;
        
    }
    return 0;
    
}
