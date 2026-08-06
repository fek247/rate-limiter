#pragma once
#include "algorithm.h"
#include "time_unit.h"

namespace RateLimiter {
    struct RateLimitConfig {
       Algorithm algorithm{Algorithm::TOKEN_BUCKET};
       unsigned int limit{100};
       unsigned int period;
       unsigned int fill_rate;
       TimeUnit time_unit{TimeUnit::SECOND};
    };
}