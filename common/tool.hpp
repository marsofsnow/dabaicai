#pragma once
#include <string>
#include <eosio/fixed_bytes.hpp>

class tool{
    public:
        static std::string to_hex(const eosio::checksum256 &hashed) {
            // Construct variables
            std::string result;
            static const char *hex_chars = "0123456789abcdef";
            const auto bytes = hashed.extract_as_byte_array();
            // Iterate hash and build result
            for (uint32_t i = 0; i < bytes.size(); ++i) {
                (result += hex_chars[(bytes.at(i) >> 4)]) += hex_chars[(bytes.at(i) & 0x0f)];
            }
            // Return string
            return result;
        }

};

