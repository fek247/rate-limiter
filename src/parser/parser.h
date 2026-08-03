#pragma once
#include "model/ratelimit_config.h"
#include <string>
#include <yaml-cpp/yaml.h>

namespace RateLimiter {
    RateLimitConfig parse_config(const std::string& filePath);
}