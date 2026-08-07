#pragma once
#include <sw/redis++/redis++.h>
#include <string>
#include <algorithm>
#include "model/algorithm.h"
#include "rate_limiter.h"
#include "model/time_unit.h"

namespace RateLimiter {
    class FixedWindowLimiter : public IRateLimiter {
        private:
            sw::redis::Redis redis_;
            int max_request_;
            long long window_period_ms_;
            std::string lua_script_;
        public:
            FixedWindowLimiter(const std::string& redis_url, int max_req, int window_period, TimeUnit time_unit)
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
                        local max_requests = tonumber(ARGV[1])
                        local window_period = tonumber(ARGV[2])
                        local count = redis.call('INCR', key)
                        if count == 1 then
                            redis.call('PEXPIRE', key, window_period)
                        end
                        return count
                    )";
                }
            bool allowRequest(const std::string& key) override {
                try {
                    auto count = redis_.command<long long>(
                        "EVAL", lua_script_, 1, key,
                        std::to_string(max_request_), 
                        std::to_string(window_period_ms_)
                    );
                    return count <= max_request_;
                } catch (const sw::redis::Error& err) {
                    std::cout << "Error when excute redis command:" << err.what() << std::endl;
                    throw;
                }
            }
    };
}