#include <iostream>

#include <seal/seal.h>

#include <json/json.h>
#include <filesystem>
#include <fstream>


#include "utils.h"

using namespace seal;
using namespace std;

int main() {
    cout << "[CLIENT]" << endl;

    // load the parameters from the server
    Json::Value params_json = get_request("http://localhost:8080/params");
    if (!params_json["success"].asBool()) {
        cerr << "Failed to load params from server: " << params_json["error"] << endl;
        exit(1);
    }

    EncryptionParameters params;
    stringstream params_stream = hex_to_sstream(params_json["params_serialized"].asString());
    params.load(params_stream);

    SEALContext context(params);

    SecretKey secret_key;
    KeyGenerator keygen(context);

    if (filesystem::exists("secret_key.bin")) {
        ifstream secret_key_ifs("secret_key.bin");
        secret_key.load(context, secret_key_ifs);
        cout << "Loaded secret key from file" << endl;
    } else {
        secret_key = keygen.secret_key();
        ofstream secret_key_ofs("secret_key.bin");
        secret_key.save(secret_key_ofs, seal::compr_mode_type::zlib);
        cout << "Generated new secret key" << endl;
    }

    PublicKey public_key;
    keygen.create_public_key(public_key);

    return 0;
}
