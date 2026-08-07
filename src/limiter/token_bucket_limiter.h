#pragma once
#include <sw/redis++/redis++.h>
#include <cmath>
#include "rate_limiter.h"
#include "model/time_unit.h"

namespace RateLimiter {
    class TokenBucketLimiter : public IRateLimiter {
        private:
            sw::redis::Redis redis_;
            int max_token_;
            long long fill_interval_ms_;
            int fill_rate_;
            long long ttl_ms_;
            std::string lua_script_;
        public:
            TokenBucketLimiter(const std::string& redis_url, int max_token, int fill_interval, int fill_rate, TimeUnit time_unit) : 
                redis_(redis_url), max_token_(max_token), fill_rate_(fill_rate)
            {
                if (time_unit == TimeUnit::MILLISECOND) {
                    fill_interval_ms_ = 1LL * fill_interval;
                } else if (time_unit == TimeUnit::SECOND) {
                    fill_interval_ms_ = 1000LL * fill_interval;
                } else if (time_unit == TimeUnit::MINUTE) {
                    fill_interval_ms_ = 60LL * 1000LL * fill_interval;
                } else {
                    fill_interval_ms_ = 3600LL * 1000LL * fill_interval;
                }

                ttl_ms_ = std::ceil((static_cast<double>(max_token) / fill_rate) * fill_interval_ms_);

                lua_script_ = R"(
                    local key = KEYS[1]
                    local now = tonumber(ARGV[1]);
                    local max_token = tonumber(ARGV[2])
                    local fill_interval = tonumber(ARGV[3])
                    local fill_rate = tonumber(ARGV[4])
                    local ttl = tonumber(ARGV[5])

                    local exists = redis.call('EXISTS', key)
                    if exists == 0 then
                        redis.call("HSET", key, "token", max_token - 1, "timestamp", now)
                        redis.call('PEXPIRE', key, ttl)
                        return 1
                    else
                        local data = redis.call("HMGET", key, "timestamp", "token")
                        local prev_time = tonumber(data[1])
                        local token = tonumber(data[2])

                        local distant = math.max(0, now - prev_time)
                        local num_token_to_add = (distant / fill_interval) * fill_rate
                        
                        local total_token = math.min(max_token, token + num_token_to_add)
                        if total_token >= 1 then
                            redis.call("HSET", key, "token", total_token - 1, "timestamp", now)
                            redis.call('PEXPIRE', key, ttl)
                            return 1
                        else
                            return 0
                        end
                    end
                )";
            };
            bool allowRequest(const std::string& key) override {
                try {
                    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();

                    auto is_allow = redis_.command<long long>(
                        "EVAL", lua_script_, 1, key,
                        std::to_string(now),
                        std::to_string(max_token_), 
                        std::to_string(fill_interval_ms_),
                        std::to_string(fill_rate_),
                        std::to_string(ttl_ms_)
                    );
                    return is_allow == 1;
                } catch (const sw::redis::Error& err) {
                    std::cout << "Error when excute redis command:" << err.what() << std::endl;
                    throw;
                }
            }
    };
}