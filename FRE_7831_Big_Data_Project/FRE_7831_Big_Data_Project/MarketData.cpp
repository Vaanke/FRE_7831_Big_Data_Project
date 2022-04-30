//
//  MarketData.cpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Elaine Huang on 2022/4/10.
//  Copyright © 2022 NYU. All rights reserved.
//

#include <stdio.h>
#include "MarketData.h"
#include <string>
#include "json/json.h"
#include "curl/curl.h"
#include <fstream>
#include <iostream>


int PullMarketData(const std::string& url_request, std::string& read_buffer)
{
    //string readBuffer;
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
    
    //cout << read_buffer << endl;
    curl_easy_cleanup(handle);
    curl_global_cleanup();
    
    return 0;
}

int GetPairsVec(const char *infile, std::vector<std::pair<std::string,std::string>> &PairVec)
{
    std::ifstream fin(infile);
    
    if (!fin.is_open())
    {
        std::cerr << "ERROR: Unable to open file." << infile << std::endl;
        return -1;
    }
    
    std::string line;
    while (getline(fin, line))
    {
        std::istringstream iss(line);
        std::string symbol1;
        std::string symbol2;
        
        getline(iss, symbol1, ',');
        getline(iss, symbol2, ',');
        
        if (!symbol2.empty() && symbol2[symbol2.length() - 1] == '\r') symbol2.erase(symbol2.size() - 1);
        
        PairVec.push_back(std::pair<std::string,std::string> { symbol1, symbol2 });
    }
    
    return 0;
}
