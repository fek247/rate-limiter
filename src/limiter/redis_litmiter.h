#pragma once
#include <sw/redis++/redis++.h>
#include <string>
#include "model/algorithm.h"
#include "rate_limiter.h"
#include <algorithm>

namespace RateLimiter {
    class RedisLimiter : public IRateLimiter {
        private:
            sw::redis::Redis redis_;
            int max_request_;
            int window_second_;
            const std::string lua_script_;
            Algorithm algorithm_;
        public:
            RedisLimiter(const std::string& redis_url, int max_req, int window_sec, Algorithm algorithm);
            bool allowRequest(const std::string& clientIp) override;
    };
}