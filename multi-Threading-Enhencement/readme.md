
# this file is the complete code for pair trading with Multithreading enhencement.

## enhenced in two part

- loading data 
- write to database.

**For the writing to db, we need to change the OpenDB configuration as the following**

    The sqlite3_open_v2() interface works like sqlite3_open() except that it accepts two additional parameters for additional control over the new database connection. The flags parameter to sqlite3_open_v2() must include, at a minimum, one of the following three flag combinations:

    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
    The database is opened for reading and writing, and is created if it does not already exist. This is the behavior that is always used for sqlite3_open() and sqlite3_open16().
    In addition to the required flags, the following optional flags are also supported:

    SQLITE_OPEN_FULLMUTEX
    The new database connection will use the "serialized" threading mode. This means the multiple threads can safely attempt to use the same database connection at the same time. (Mutexes will block any actual concurrency, but in this mode there is no harm in trying.)


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
