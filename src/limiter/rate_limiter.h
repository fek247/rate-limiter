#pragma once
#include <string>

namespace RateLimiter {
    class IRateLimiter {
        public:
            virtual ~IRateLimiter() = default;

            virtual bool allowRequest(const std::string& key) = 0;
    };
}
