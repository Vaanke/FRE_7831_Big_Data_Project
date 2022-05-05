
# this file is the complete code for pair trading with Multithreading enhencement.

## enhenced in two part

- loading data 
- write to database.

**For the writing to db, we need to change the OpenDB configuration as the following**

```c++ 
int OpenDatabase(const char* database_name, sqlite3* & db)
{
    int rc = 0;
    rc = sqlite3_open_v2(database_name, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, NULL);
    if (rc)
    {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }
    else
    {
        std::cout << "Opened " << database_name << std::endl;
    }

    std::string foreign_ley_check = "PRAGMA foreign_keys = ON";
    if (sqlite3_exec(db, foreign_ley_check.c_str(), 0, 0, 0) != SQLITE_OK) {
        std::cout << "SQLite fails in setting foreign key check" << std::endl;
        sqlite3_close(db);
        return -1;
    }

    return 0;
}
```
