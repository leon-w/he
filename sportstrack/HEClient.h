#pragma once

#include "seal/seal.h"
#include <vector>
#include <string>
#include <sstream>

#include "utils.h"

class HEClient {
public:
    explicit HEClient(const seal::EncryptionParameters &params) :
            params(params), context(params), batch_encoder(context) {
        size_t params_hash = std::hash<seal::EncryptionParameters>{}(params);
        std::string secret_key_file_name =
                std::string("secret_key_") + std::to_string(params_hash) + std::string(".bin");
        std::string public_key_file_name =
                std::string("public_key_") + std::to_string(params_hash) + std::string(".bin");

        seal::KeyGenerator keygen(context);
        if (std::filesystem::exists(secret_key_file_name) && std::filesystem::exists(public_key_file_name)) {
            std::ifstream secret_key_ifs(secret_key_file_name);
            std::ifstream public_key_ifs(public_key_file_name);
            secret_key.load(context, secret_key_ifs);
            public_key.load(context, public_key_ifs);
            std::cout << "[i] Loaded keys from file." << std::endl;
        } else {
            secret_key = keygen.secret_key();
            keygen.create_public_key(public_key);
            std::ofstream secret_key_ofs(secret_key_file_name);
            std::ofstream public_key_ofs(public_key_file_name);
            secret_key.save(secret_key_ofs, seal::compr_mode_type::zlib);
            public_key.save(public_key_ofs, seal::compr_mode_type::zlib);
            std::cout << "[i] Generated new keys." << std::endl;
        }

        encryptor = new seal::Encryptor(context, public_key);
        decryptor = new seal::Decryptor(context, secret_key);
    }

    ~HEClient() {
        delete encryptor;
        delete decryptor;
    }

    seal::Ciphertext encrypted(const std::vector<uint64_t> &data) const {
        seal::Plaintext encoded;
        batch_encoder.encode(data, encoded);

        seal::Ciphertext ciphertext;
        encryptor->encrypt(encoded, ciphertext);
        return ciphertext;
    }

    std::vector<uint64_t> decrypt(const seal::Ciphertext &ciphertext) {
        seal::Plaintext encoded;
        decryptor->decrypt(ciphertext, encoded);

        std::vector<uint64_t> data;
        batch_encoder.decode(encoded, data);
        return data;
    }

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

private:
    seal::EncryptionParameters params;
    seal::SEALContext context;
    seal::SecretKey secret_key;
    seal::PublicKey public_key;
    seal::BatchEncoder batch_encoder;
    seal::Encryptor *encryptor;
    seal::Decryptor *decryptor;
};
