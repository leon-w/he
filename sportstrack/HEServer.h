#pragma once

#include "seal/seal.h"
#include <vector>
#include <string>
#include <sstream>

#include "utils.h"

class HEServer {
public:
    explicit HEServer(const seal::EncryptionParameters &params) : context(params), evaluator(context) {}

    static std::string serialize_ciphertext(const seal::Ciphertext &ciphertext) {
        std::stringstream ss;
        ciphertext.save(ss, seal::compr_mode_type::zlib);
        return sstream_to_hex(ss);
    }

    seal::Ciphertext parse_ciphertext(const std::string &str) {
        seal::Ciphertext ciphertext;
        std::stringstream ss = hex_to_sstream(str);
        ciphertext.load(context, ss);
        return ciphertext;
    }

    seal::Ciphertext add_ciphertexts(const seal::Ciphertext &c1, const seal::Ciphertext &c2) {
        seal::Ciphertext result;
        evaluator.add(c1, c2, result);
        return result;
    }

private:
    seal::EncryptionParameters params;
    seal::SEALContext context;
    seal::Evaluator evaluator;
};
