#include "Database.h"

#include <iostream>
#include <sstream>
#include <chrono>

#include <json/value.h>

void execute_sql(sqlite3 *db, const char *sql) {
    char *sql_error_msg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &sql_error_msg);

    if (rc != SQLITE_OK) {
        std::cerr << "Error executing SQL: " << sql_error_msg << std::endl;
        sqlite3_free(sql_error_msg);
        exit(1);
    }
}

static int execute_sql_json_callback(void *data, int argc, char **argv, char **col_names) {
    auto result = static_cast<Json::Value *>(data);
    for (int i = 0; i < argc; i++) {
        result->operator[](col_names[i])[result->operator[](col_names[i]).size()] = argv[i];
    }
    return 0;
}


Json::Value execute_sql_json(sqlite3 *db, const char *sql) {
    Json::Value result;

    char *sql_error_msg = nullptr;
    int rc = sqlite3_exec(db, sql, execute_sql_json_callback, &result, &sql_error_msg);

    if (rc != SQLITE_OK) {
        std::cerr << "Error executing SQL: " << sql_error_msg << std::endl;
        sqlite3_free(sql_error_msg);
        exit(1);
    }

    return result;
}


Database::~Database() {
    sqlite3_close(db);
}

Database::Database(const std::string &database_file) {
    int rc = sqlite3_open(database_file.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(1);
    }

    char create_tables_sql[] =
            "CREATE TABLE IF NOT EXISTS REPETITIONS"
            "   (ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    CIPHERTEXT CHAR,"
            "    TIMESTAMP INT);"
            "CREATE TABLE IF NOT EXISTS REPETITIONS_SUM"
            "   (CIPHERTEXT CHAR);";

    execute_sql(db, create_tables_sql);
}

void Database::write_repetition(const std::string &hex_encoded) {
    std::stringstream ss;

    ss << "INSERT INTO REPETITIONS (CIPHERTEXT, TIMESTAMP) ";
    ss << "VALUES (";
    ss << "'" << hex_encoded << "',";
    ss << std::time(nullptr);
    ss << ");";

    execute_sql(db, ss.str().c_str());
}

void Database::write_repetitions_sum(const std::string &hex_encoded, bool first_commit) {
    std::stringstream ss;

    if (first_commit) {
        ss << "INSERT INTO REPETITIONS_SUM (CIPHERTEXT) ";
        ss << "VALUES (";
        ss << "'" << hex_encoded << "');";
    } else {
        ss << "UPDATE REPETITIONS_SUM set CIPHERTEXT = ";
        ss << "'" << hex_encoded << "';";
    }

    execute_sql(db, ss.str().c_str());
}

Json::Value Database::get_repetitions(bool truncate) {
    std::stringstream ss;
    if (truncate) {
        ss << R"(SELECT "[" || LENGTH(CIPHERTEXT) || " B] " || SUBSTR(CIPHERTEXT, 1, 32) || "..." as CIPHERTEXT,)";
        ss << "TIMESTAMP FROM REPETITIONS;";
    } else {
        ss << "SELECT CIPHERTEXT, TIMESTAMP FROM REPETITIONS;";
    }

    return execute_sql_json(db, ss.str().c_str());
}

std::string Database::get_repetitions_sum(bool truncate) {
    std::string query;
    if (truncate) {
        query = R"(SELECT "[" || LENGTH(CIPHERTEXT) || " B] " || SUBSTR(CIPHERTEXT, 1, 32) || "..." as CIPHERTEXT FROM REPETITIONS_SUM;)";;
    } else {
        query = "SELECT CIPHERTEXT FROM REPETITIONS_SUM;";
    }

    Json::Value result = execute_sql_json(db, query.c_str());
    return result["CIPHERTEXT"][0].asString();
}