#include <iostream>

#include <seal/seal.h>

#include <json/json.h>
#include <filesystem>
#include <fstream>


#include "utils.h"
#include "request_utils.h"
#include "exercises.h"
#include "HEClient.h"

using namespace seal;
using namespace std;


EncryptionParameters load_params_from_server(const string &url) {
    Json::Value params_json = get_request(url);
    if (!params_json["success"].asBool()) {
        cerr << "Failed to load params from server: " << params_json["error"] << endl;
        exit(1);
    }

    EncryptionParameters params;
    stringstream params_stream = hex_to_sstream(params_json["params_serialized"].asString());
    params.load(params_stream);
    return params;
}

void get_repetitions_sum_from_server(const string &url, HEClient &client) {
    Json::Value sum_json = get_request(url);
    if (!sum_json["success"].asBool()) {
        cerr << "Failed to load sum from server: " << sum_json["error"] << endl;
        exit(1);
    }

    Ciphertext encrypted_sum_vec = client.parse_ciphertext(sum_json["sum_ciphertext"].asString());
    vector<uint64_t> sum_vec_decoded = client.decrypt(encrypted_sum_vec);

    cout << "Your total exercise count:" << endl;
    for (int i = 0; i < exercises.size(); i++) {
        cout << " " << i + 1 << ". " << exercises[i] << ": " << sum_vec_decoded[i] << endl;
    }
    cout << "----------------------------" << endl;
}

void push_repetitions_to_server(const string &url, const Ciphertext &encrypted_repetitions_vec) {
    Json::Value payload;
    payload["repetitions_ciphertext"] = HEClient::serialize_ciphertext(encrypted_repetitions_vec);

    Json::Value response = post_request(url, stringify_json(payload));

    if (!response["success"].asBool()) {
        cerr << "Failed to push exercises to server: " << response["error"] << endl;
        exit(1);
    }
}

pair<int, int> prompt_exercise_count() {
    for (int i = 0; i < exercises.size(); i++) {
        cout << " " << i + 1 << ". " << exercises[i] << endl;
    }

    int exercise = read_int("Which exercise did you do?", 1, exercises.size());
    int repetitions = read_int("How many repetitions did you do?", 0, 1000);

    return {exercise, repetitions};
}

int main() {
    cout << "--- [SportsTrack CLIENT] ---" << endl;

    // load the parameters from the server
    EncryptionParameters params = load_params_from_server("http://localhost:8080/api/params");
    HEClient client(params);

    while (true) {
        cout << endl;
        auto[exercise, repetitions] = prompt_exercise_count();
        vector<uint64_t> repetitions_vec(exercises.size(), 0);
        repetitions_vec[exercise - 1] = repetitions;

        Ciphertext encrypted_repetitions_vec = client.encrypted(repetitions_vec);

        push_repetitions_to_server("http://localhost:8080/api/push_repetitions", encrypted_repetitions_vec);

        cout << endl;
        get_repetitions_sum_from_server("http://localhost:8080/api/get_sum", client);
    }

    return 0;
}
