#include <iostream>

#include <seal/seal.h>
#include <httpserver.hpp>
#include <json/json.h>

#include "parameters.h"


using namespace seal;
using namespace std;
using namespace httpserver;

const EncryptionParameters params = get_encryption_params();


class params_resource : public http_resource {
public:
    const shared_ptr<http_response> render(const http_request &) override {
        Json::Value root;
        root["success"] = true;
        root["params_serialized"] = serialize_params(params);
        Json::StreamWriterBuilder builder;
        const string response = Json::writeString(builder, root);
        return std::shared_ptr<http_response>(new string_response(response));
    }
};

int main() {
    cout << "[SERVER]" << endl;

    webserver ws = create_webserver(8080);

    params_resource params_resource;
    ws.register_resource("/params", &params_resource);

    SEALContext context(params);

    ws.start(true);
    return 0;
}
