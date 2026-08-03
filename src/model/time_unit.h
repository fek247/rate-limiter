#pragma once
#include <stdexcept>

namespace RateLimiter {
    enum class TimeUnit {
        SECOND,
        MILLISECOND,
        MINUTE,
        HOUR,
    };

    inline TimeUnit stringToTimeUnit(const std::string& str) {
        static const std::unordered_map<std::string, TimeUnit> stringToEnumMap = {
            {"millisecond", TimeUnit::MILLISECOND},
            {"second", TimeUnit::SECOND},
            {"minute", TimeUnit::MINUTE},
            {"hour", TimeUnit::HOUR},
        };

        auto it = stringToEnumMap.find(str);
        if (it != stringToEnumMap.end()) {
            return it->second;
        }

        throw std::invalid_argument("Unknown enum string: " + str);
    };
}