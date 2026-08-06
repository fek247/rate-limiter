#pragma once
#include <iostream>
#include <string>
#include "parser/parser.h"
#include "limiter/rate_limiter.h"
#include "model/algorithm.h"
#include "fixed_window_limiter.h"
#include "sliding_window_limiter.h"
#include "token_bucket_limiter.h"

namespace RateLimiter {
    class RateLimiterFactory {
        public:
            static std::unique_ptr<IRateLimiter> create(Algorithm algorithm, const std::string& redis_url, const RateLimitConfig& config) {
                switch (algorithm)
                {
                    case Algorithm::FIXED_WINDOW_COUNTER:
                        return std::make_unique<FixedWindowLimiter>(redis_url, config.limit, config.period);
                    case Algorithm::SLIDING_WINDOW_COUNTER:
                        return std::make_unique<SlidingWindowLimiter>(redis_url, config.limit, config.period);
                    case Algorithm::TOKEN_BUCKET:
                        return std::make_unique<TokenBucketLimiter>(redis_url, config.limit, config.period, config.fill_rate);
                    default:
                        break;
                }

                return nullptr;
            }
    };
}