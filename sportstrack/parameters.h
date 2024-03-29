#pragma once

#include "seal/seal.h"

#include "utils.h"


inline seal::EncryptionParameters
get_encryption_params(std::size_t poly_modulus_degree = 8192) {
    seal::EncryptionParameters params(seal::scheme_type::bfv);
    params.set_poly_modulus_degree(poly_modulus_degree);
    params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));
    return params;
}

inline std::string serialize_params(const seal::EncryptionParameters &params) {
    std::stringstream sstream;
    params.save(sstream, seal::compr_mode_type::zlib);
    return sstream_to_hex(sstream);
}