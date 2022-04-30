//
//  Database.hpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/28/22.
//
//

#pragma once
#include <string>
#include "sqlite3.h"
using namespace std;

int OpenDatabase(const char* database_name, sqlite3* & db);

int DropTable(sqlite3 * db, const char* sql_stmt);

int ExecuteSQL(sqlite3* db, const char* sql_stmt);

int ShowTable(sqlite3* db, const char* sql_stmt);

void CloseDatabase(sqlite3* db);

string sql_stmt_insert_Daily(string symbol, string date, float open, float high, float low, float close, float adjusted_close, int volume);
string sql_stmt_insert_Intraday(string symbol, string date, string timestamp, float open, float high, float low, float close, int volume);
