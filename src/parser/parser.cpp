#include "parser.h"

namespace RateLimiter {
    RateLimitConfig parse_config(const std::string& filePath) {
        YAML::Node node;
        try {
            node = YAML::LoadFile(filePath);
        } catch (const YAML::BadFile&) {
            throw std::runtime_error("File not found or cannot be opened " + filePath);
        } catch (const YAML::Exception& e) {
            throw std::runtime_error("Yaml parsing error in " + filePath + ": " + e.msg);
        }

        RateLimitConfig config;
        if (node["algorithm"]) {
            config.algorithm = stringToAlgorithm(node["algorithm"].as<std::string>());
        }
        if (node["limit"]) {
            config.limit = node["limit"].as<unsigned int>();
        }
        if (node["period"]) {
            config.period = node["period"].as<unsigned int>();
        }
        if (node["time_unit"]) {
            config.time_unit = stringToTimeUnit(node["time_unit"].as<std::string>());
        }
        if (node["fill_rate"]) {
            config.fill_rate = node["fill_rate"].as<unsigned int>();
        }

        return config;
    }
}