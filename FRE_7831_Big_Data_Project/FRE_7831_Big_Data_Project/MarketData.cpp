//
//  MarketData.cpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Elaine Huang on 2022/4/10.
//  Copyright © 2022 NYU. All rights reserved.
//


#include "MarketData.h"


//int PullMarketData(const std::string& url_request, std::string& read_buffer)
//{
//    string sConfigFile = "config_mac.csv";
//    map<string, string> config_map = ProcessConfigData(sConfigFile);
//
//    //string readBuffer;
//
//    //global initiliation of curl before calling a function
//    curl_global_init(CURL_GLOBAL_ALL);
//
//    //creating session handle
//    CURL * handle;
//
//    // Store the result of CURLís webpage retrieval, for simple error checking.
//    CURLcode result;
//
//    // notice the lack of major error-checking, for brevity
//    handle = curl_easy_init();
//
////fprintf(stderr, "curl_easy_init() failed: %s\n", handle);
//
//
//    if (!handle)
//    {
//        cout << "curl_easy_init failed" << endl;
//        return -1;
//    }
//
//    //after creating handle we will start transfering webpage
//    //curl_easy_setopt is used to tell libcurl how to behave.
//    //By setting the appropriate options, the application can change libcurl's behavior.
//
//    curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
//    //cout<< url_request.c_str();
//
//
//    //adding a user agent
//    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
//    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
//    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
//
//    // send all data to this function
//    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
//
//    // we pass our 'chunk' struct to the callback function
//    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &read_buffer);
//
//    //perform a blocking file transfer
//    result = curl_easy_perform(handle);
//
//    // check for errors
//    if (result != CURLE_OK) {
//        cout << "curl_easy_perform failed" << endl;
//        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
//        return -1;
//    }
//
//    //cout << read_buffer << endl;
//    curl_easy_cleanup(handle);
//    return 0;
//}
//
//int PopulateDailyTrades(const std::string& read_buffer,Stock& stock)
//{
//    //json parsing
//    Json::CharReaderBuilder builder;
//    Json::CharReader* reader = builder.newCharReader();
//    Json::Value root;   // will contains the root value after parsing.
//    string errors;
//
//    bool parsingSuccessful = reader->parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), &root, &errors);
//    if (not parsingSuccessful)
//    {
//        // Report failures and their locations in the document.
//        cout << "Failed to parse JSON" << endl << read_buffer << errors << endl;
//        return -1;
//    }
//
//    cout << "\nSucess parsing json\n" << root << endl;
//    string datetime;
//    float open, high, low, close,adjusted_close;
//    int volume;
//
//    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
//    {
//        datetime = (*itr)["date"].asString();
//        open = (*itr)["open"].asFloat();
//        high = (*itr)["high"].asFloat();
//        low = (*itr)["low"].asFloat();
//        close = (*itr)["close"].asFloat();
//        adjusted_close = (*itr)["adjusted_close"].asFloat();
//        volume = (*itr)["volume"].asInt64();
//        DailyTrade aTrade(datetime, open, high, low, close, adjusted_close, volume);
//        stock.addDailyTrade(aTrade);
//    }
//    return 0;
//}
//
//int PopulateIntradayTrades(const std::string& read_buffer, Stock& stock, long start_date)
//{
//    //json parsing
//    Json::CharReaderBuilder builder;
//    Json::CharReader* reader = builder.newCharReader();
//    Json::Value root;   // will contains the root value after parsing.
//    string errors;
//
//    bool parsingSuccessful = reader->parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), &root, &errors);
//    if (not parsingSuccessful)
//    {
//        // Report failures and their locations in the document.
//        cout << "Failed to parse JSON" << endl << read_buffer << errors << endl;
//        return -1;
//    }
//
//    cout << "\nSucess parsing json\n" << root << endl;
//    string date;
//    string timestamp;
//    float open, high, low, close;
//    long volume;
//
//    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
//    {
//        long trade_timestamp = (*itr)["timestamp"].asInt64();
//        if (trade_timestamp <= start_date)
//            continue;
//        string datetime = (*itr)["datetime"].asString();
//        size_t current, previous = 0;
//        current = datetime.find(' ');
//        date = datetime.substr(previous, current - previous);
//        previous = current + 1;
//        string temp = datetime.substr(previous, datetime.length() - current);
//        previous = 0;
//        current = temp.find(':');
//        int hour = stoi(temp.substr(previous, current - previous));
//        hour -= 5;// convert to local time
//        previous = current + 1;
//        string mins_second = temp.substr(previous, datetime.length() - current);
//        timestamp = to_string(hour) + ":" + mins_second;
//        open = (*itr)["open"].asFloat();
//        high = (*itr)["high"].asFloat();
//        low = (*itr)["low"].asFloat();
//        close = (*itr)["close"].asFloat();
//        volume = (*itr)["volume"].asInt64();
//        IntradayTrade aTrade(date.c_str(), timestamp.c_str(), open, high, low, close, volume);
//        stock.addIntradayTrade(aTrade);
//    }
//
//    return 0;
//
//}
