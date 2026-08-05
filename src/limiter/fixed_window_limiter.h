#pragma once
#include <sw/redis++/redis++.h>
#include <string>
#include "model/algorithm.h"
#include "rate_limiter.h"
#include <algorithm>

namespace RateLimiter {
    class FixedWindowLimiter : public IRateLimiter {
        private:
            sw::redis::Redis redis_;
            int max_request_;
            int window_second_;
            std::string lua_script_;
        public:
            FixedWindowLimiter(const std::string& redis_url, int max_req, int window_sec)
                : redis_(redis_url), max_request_(max_req), window_second_(window_sec) {
                    lua_script_ = R"(
                        local key = KEYS[1]
                        local max_requests = tonumber(ARGV[1])
                        local window_seconds = tonumber(ARGV[2])
                        local count = redis.call('INCR', key)
                        if count == 1 then
                            redis.call('EXPIRE', key, window_seconds)
                        end
                        return { count }
                    )";
                }
            bool allowRequest(const std::string& key) override {
                auto count = redis_.command<long long>(
                    "EVAL", lua_script_, 1, key, 
                    std::to_string(max_request_), 
                    std::to_string(window_second_)
                );
                return count <= max_request_;
            }
    };
}