#pragma once
#include <sw/redis++/redis++.h>
#include "rate_limiter.h"

namespace RateLimiter {
    class TokenBucketLimiter : public IRateLimiter {
        private:
            sw::redis::Redis redis_;
            int max_token_;
            int fill_interval_;
            int fill_rate_;
            std::string lua_script_;
        public:
            TokenBucketLimiter(const std::string& redis_url, int max_token, int fill_interval, int fill_rate) : 
                redis_(redis_url), max_token_(max_token), fill_interval_(fill_interval), fill_rate_(fill_rate) 
            {
                lua_script_ = R"(
                    local key = KEYS[1]
                    local now = tonumber(ARGV[1]);
                    local max_token = tonumber(ARGV[2])
                    local fill_interval = tonumber(ARGV[3])
                    local fill_rate = tonumber(ARGV[4])
                    local ttl = math.ceil((max_token / fill_rate) * fill_interval)

                    local exists = redis.call('EXISTS', key)
                    if exists == 0 then
                        redis.call("HSET", key, "token", max_token - 1, "timestamp", now)
                        redis.call('EXPIRE', key, ttl)
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
                            redis.call("EXPIRE", key, ttl)
                            return 1
                        else
                            return 0
                        end
                    end
                )";
            };
            bool allowRequest(const std::string& key) override {
                long long now_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                auto is_allow = redis_.command<long long>(
                    "EVAL", lua_script_, 1, key,
                    std::to_string(now_seconds),
                    std::to_string(max_token_), 
                    std::to_string(fill_interval_),
                    std::to_string(fill_rate_)
                );
                return is_allow == 1;
            }
    };
}