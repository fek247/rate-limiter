#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace RateLimiter {
    enum class Algorithm {
        TOKEN_BUCKET,
        LEAKING_BUCKET,
        FIXED_SLIDING_WINDOWS,
    };

    inline Algorithm stringToAlgorithm(const std::string& str) {
        static const std::unordered_map<std::string, Algorithm> stringToEnumMap = {
            {"token_bucket", Algorithm::TOKEN_BUCKET},
            {"leaking_bucket", Algorithm::LEAKING_BUCKET},
            {"fixed_sliding_windows", Algorithm::FIXED_SLIDING_WINDOWS},
        };

        auto it = stringToEnumMap.find(str);
        if (it != stringToEnumMap.end()) {
            return it->second;
        }

        throw std::invalid_argument("Unknown enum string: " + str);
    };
}