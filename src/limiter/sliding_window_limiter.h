#pragma once
#include <sw/redis++/redis++.h>
#include <string>
#include "model/algorithm.h"
#include "rate_limiter.h"
#include <algorithm>

namespace RateLimiter {
    class SlidingWindowLimiter : public IRateLimiter {
        private:
            sw::redis::Redis redis_;
            int max_request_;
            int window_second_;
            std::string lua_script_;
        public:
            SlidingWindowLimiter(const std::string& redis_url, int max_req, int window_sec)
                : redis_(redis_url), max_request_(max_req), window_second_(window_sec) {
                    lua_script_ = R"(
                        local key = KEYS[1]
                        local now = tonumber(ARGV[1]);
                        local window_size = tonumber(ARGV[2])
                        local window_seconds = tonumber(ARGV[3])
                        
                        local current_window_key = key .. ":" .. math.floor(now / window_seconds)
                        local previous_window_key = key .. ":" .. math.floor((now - window_seconds) / window_seconds)

                        local current_count = tonumber(redis.call("GET", current_window_key) or "0")
                        local previous_count = tonumber(redis.call("GET", previous_window_key) or "0")

                        local time_into_window = now % window_size
                        local weight = (window_size - time_into_window) / window_size

                        local estimated_count = math.floor(previous_count * weight + current_count)

                        if estimated_count >= window_size then
                            return 0
                        else
                            redis.call("INCR", current_window_key)
                            redis.call("EXPIRE", current_window_key, window_size * 2)
                            return 1
                        end
                    )";
                }
            bool allowRequest(const std::string& key) override {
                long long now_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                auto is_allow = redis_.command<long long>(
                    "EVAL", lua_script_, 1, key,
                    std::to_string(now_seconds),
                    std::to_string(max_request_), 
                    std::to_string(window_second_)
                );
                return is_allow == 1;
            }
    };
}