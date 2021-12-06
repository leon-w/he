#pragma once

#include <cstdint>
#include <sstream>
#include <iomanip>

#include <seal/seal.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <json/json.h>

inline std::string uint64_to_hex(std::uint64_t value) {
    return seal::util::uint_to_hex_string(&value, std::size_t(1));
}

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
        result << (char) strtol(byteString.c_str(), NULL, 16);
    }
    return result;
}

Json::Value parse_json(std::istream &is) {
    Json::Value root;
    Json::CharReaderBuilder builder;

    JSONCPP_STRING errs;
    if (!parseFromStream(builder, is, &root, &errs)) {
        std::cerr << "FAILED TO PARSE JSON: " << errs << std::endl;
        exit(1);
    }
    return root;
}

Json::Value get_request(const std::string& url) {
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(url));

        std::stringstream ss;
        ss << request;
        return parse_json(ss);
    }
    catch (const std::exception &e) {
        Json::Value root;
        root["success"] = false;
        root["error"] = e.what();
        return root;
    }
}