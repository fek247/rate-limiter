#pragma once
#include <sw/redis++/redis++.h>
#include <string>
#include <algorithm>
#include "model/time_unit.h"
#include "model/algorithm.h"
#include "rate_limiter.h"
namespace RateLimiter {
    class SlidingWindowLimiter : public IRateLimiter {
        private:
            sw::redis::Redis redis_;
            int max_request_;
            long long window_period_ms_;
            std::string lua_script_;
        public:
            SlidingWindowLimiter(const std::string& redis_url, int max_req, int window_period, TimeUnit time_unit)
                : redis_(redis_url), max_request_(max_req) {
                    if (time_unit == TimeUnit::MILLISECOND) {
                        window_period_ms_ = 1LL * window_period;
                    } else if (time_unit == TimeUnit::SECOND) {
                        window_period_ms_ = 1000LL * window_period;
                    } else if (time_unit == TimeUnit::MINUTE) {
                        window_period_ms_ = 60LL * 1000LL * window_period;
                    } else {
                        window_period_ms_ = 3600LL * 1000LL * window_period;
                    }

                    lua_script_ = R"(
                        local key = KEYS[1]
                        local now = tonumber(ARGV[1]);
                        local max_request = tonumber(ARGV[2])
                        local window_period = tonumber(ARGV[3])
                        
                        local current_window_key = key .. ":" .. math.floor(now / window_period)
                        local previous_window_key = key .. ":" .. math.floor((now - window_period) / window_period)

                        local current_count = tonumber(redis.call("GET", current_window_key) or "0")
                        local previous_count = tonumber(redis.call("GET", previous_window_key) or "0")

                        local time_into_window = now % window_period
                        local weight = (window_period - time_into_window) / window_period

                        local estimated_count = math.floor(previous_count * weight + current_count)

                        if estimated_count >= max_request then
                            return 0
                        else
                            redis.call("INCR", current_window_key)
                            redis.call("PEXPIRE", current_window_key, window_period * 2)
                            return 1
                        end
                    )";
                }
            bool allowRequest(const std::string& key) override {
                try {
                    long long now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();

                    auto is_allow = redis_.command<long long>(
                        "EVAL", lua_script_, 1, key,
                        std::to_string(now_ms),
                        std::to_string(max_request_),
                        std::to_string(window_period_ms_)
                    );
                    return is_allow == 1;
                } catch (const sw::redis::Error& err) {
                    std::cout << "Error when excute redis command:" << err.what() << std::endl;
                    throw;
                }
            }
    };
}