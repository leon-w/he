#pragma once

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "utils.h"

Json::Value get_request(const std::string &url) {
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(url));

        std::stringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        return parse_json(response);
    }
    catch (const std::exception &e) {
        Json::Value root;
        root["success"] = false;
        root["error"] = e.what();
        return root;
    }
}

Json::Value post_request(const std::string &url, const std::string &body) {
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(url));
        request.setOpt(new curlpp::options::PostFields(body));
        request.setOpt(new curlpp::options::PostFieldSize(body.length()));

        std::stringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        return parse_json(response);
    }
    catch (const std::exception &e) {
        Json::Value root;
        root["success"] = false;
        root["error"] = e.what();
        return root;
    }
}
