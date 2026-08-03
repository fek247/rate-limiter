#include "redis_litmiter.h"

namespace RateLimiter {
    RedisLimiter::RedisLimiter(const std::string& redis_url, int max_req, int window_sec, Algorithm algorithm)
    : redis_(redis_url), max_request_(max_req), window_second_(window_sec),
        lua_script_(R"(
            local key = KEYS[1]
            local max_requests = tonumber(ARGV[1])
            local window_seconds = tonumber(ARGV[2])
            local count = redis.call('INCR', key)
            if count == 1 then
                redis.call('EXPIRE', key, window_seconds)
            end
            return { count, redis.call('PTTL', key) }
        )"), algorithm_(algorithm) {}

    bool RedisLimiter::allowRequest(const std::string& client_ip) {
        auto result = redis_.command<std::vector<long long>>(
            "EVAL", lua_script_, 1, client_ip, 
            std::to_string(max_request_), 
            std::to_string(window_second_)
        );
        long long count = result[0];
        return count <= max_request_;
    }
}

