#pragma once

#include <sqlite3.h>
#include <string>

#include <json/value.h>

#include "exercises.h"

class Database {
public:
    explicit Database(const std::string &database_file); // constructor
    ~Database(); // destructor

    void write_repetition(const std::string &hex_encoded);

    Json::Value get_repetitions(bool truncate = false);

    std::string get_repetitions_sum(bool truncate = false);

    void write_repetitions_sum(const std::string &hex_encoded, bool first_commit);

private:
    sqlite3 *db{};
};
