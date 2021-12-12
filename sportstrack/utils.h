#pragma once

#include <cstdint>
#include <sstream>
#include <iomanip>

#include "seal/seal.h"

#include <json/json.h>

std::string sstream_to_hex(const std::stringstream &sstream) {
    std::stringstream result;
    result << std::hex << std::setfill('0');
    for (unsigned char c: sstream.str()) {
        result << std::setw(2) << static_cast<int>(c);
    }
    return result.str();
}

std::stringstream hex_to_sstream(const std::string &hex) {
    std::stringstream result;
    for (int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        result << (char) strtol(byteString.c_str(), nullptr, 16);
    }
    return result;
}

Json::Value parse_json(std::istream &is) {
    Json::Value root;
    Json::CharReaderBuilder builder;

    JSONCPP_STRING errs;
    if (!parseFromStream(builder, is, &root, &errs)) {
        std::cerr << "FAILED TO PARSE JSON: " << errs << std::endl;
        std::cerr << std::endl;
        exit(1);
    }
    return root;
}

inline Json::Value parse_json(const std::string &str) {
    std::stringstream ss(str);
    return parse_json(ss);
}

inline std::string stringify_json(Json::Value &json) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, json);
}


int read_int(std::string prompt, int min = 0, int max = 5) {
    while (true) {
        int result;
        std::string in;
        std::cout << prompt << " (" << min << "-" << max << ") > ";
        std::cin >> result;
        if (!std::cin.fail() && result >= min && result <= max) {
            return result;
        } else {
            std::cout << "Invalid value!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}