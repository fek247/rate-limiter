#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace RateLimiter {
    enum class Algorithm {
        TOKEN_BUCKET,
        SLIDING_WINDOW_COUNTER,
        FIXED_WINDOW_COUNTER
    };

    inline Algorithm stringToAlgorithm(const std::string& str) {
        static const std::unordered_map<std::string, Algorithm> stringToEnumMap = {
            {"token_bucket", Algorithm::TOKEN_BUCKET},
            {"sliding_window_counter", Algorithm::SLIDING_WINDOW_COUNTER},
            {"fixed_window_counter", Algorithm::FIXED_WINDOW_COUNTER},
        };

        auto it = stringToEnumMap.find(str);
        if (it != stringToEnumMap.end()) {
            return it->second;
        }

        throw std::invalid_argument("Unknown enum string: " + str);
    };
}