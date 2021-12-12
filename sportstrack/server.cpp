#include <iostream>

#include "seal/seal.h"
#include <httpserver.hpp>
#include <json/json.h>

#include "parameters.h"
#include "Database.h"
#include "HEServer.h"

using namespace seal;
using namespace std;
using namespace httpserver;

const EncryptionParameters params = get_encryption_params();
HEServer server(params);
Database db("sportstrack.db");

#define JSON_RESPONSE(x) std::shared_ptr<http_response>(new string_response(stringify_json(x)))

class params_resource : public http_resource {
public:
    const shared_ptr<http_response> render(const http_request &) override {
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["params_serialized"] = serialize_params(params);
        return JSON_RESPONSE(responseJson);
    }
};

class clear_db_resource : public http_resource {
public:
    const shared_ptr<http_response> render(const http_request &) override {
        db.clear_database();
        Json::Value responseJson;
        responseJson["success"] = true;
        return JSON_RESPONSE(responseJson);
    }
};


class get_database_dump_resource : public http_resource {
public:
    const shared_ptr<http_response> render(const http_request &) override {
        Json::Value responseJson;

        responseJson["success"] = true;
        responseJson["database"]["repetitions"] = db.get_repetitions(true);
        responseJson["database"]["repetitions_sum"] = db.get_repetitions_sum(true);
        auto response = JSON_RESPONSE(responseJson);
        response->with_header("Access-Control-Allow-Origin", "*");
        return response;
    }
};

class get_sum_resource : public http_resource {
public:
    const shared_ptr<http_response> render(const http_request &) override {
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["sum_ciphertext"] = db.get_repetitions_sum();
        return JSON_RESPONSE(responseJson);
    }
};

class push_repetitions_resource : public http_resource {
public:
    const shared_ptr<http_response> render_POST(const http_request &request) override {
        bool success = false;

        Json::Value requestJson = parse_json(request.get_content());
        if (requestJson.isMember("repetitions_ciphertext")) {
            string hex_encoded_repetitions = requestJson["repetitions_ciphertext"].asString();
            db.write_repetition(hex_encoded_repetitions);

            string repetitions_sum = db.get_repetitions_sum();

            if (repetitions_sum.empty()) {
                db.write_repetitions_sum(hex_encoded_repetitions, true);
            } else {
                Ciphertext current_sum = server.parse_ciphertext(repetitions_sum);
                Ciphertext new_repetitions = server.parse_ciphertext(hex_encoded_repetitions);
                Ciphertext new_sum = server.add_ciphertexts(current_sum, new_repetitions);
                string new_sum_hex = HEServer::serialize_ciphertext(new_sum);
                db.write_repetitions_sum(new_sum_hex, false);
            }
            success = true;
        }

        Json::Value responseJson;
        responseJson["success"] = success;
        return JSON_RESPONSE(responseJson);
    }
};

int main() {
    cout << "--- [SportsTrack SERVER] ---" << endl;

    webserver ws = create_webserver(8080);

    params_resource params_resource;
    ws.register_resource("/api/params", &params_resource);

    push_repetitions_resource push_repetitions_resource;
    ws.register_resource("/api/push_repetitions", &push_repetitions_resource);

    get_sum_resource get_sum_resource;
    ws.register_resource("/api/get_sum", &get_sum_resource);

    get_database_dump_resource get_database_dump_resource;
    ws.register_resource("/api/get_database_dump", &get_database_dump_resource);

    clear_db_resource clear_db_resource;
    ws.register_resource("/api/clear_db", &clear_db_resource);

    ws.start(true);
    return 0;
}
