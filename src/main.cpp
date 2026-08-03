#include <iostream>
#include <string>
#include <sw/redis++/redis++.h>
#include "parser/parser.h"
#include "limiter/redis_litmiter.h"
#include "network/tcp.h"
#include "model/algorithm.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::string config_file_path = (argc > 1) ? argv[1] : "config.yaml";

    try {
        auto config = RateLimiter::parse_config(config_file_path);

        RateLimiter::RedisLimiter limiter("tcp://127.0.0.1:6379", config.limit, config.period, RateLimiter::Algorithm::TOKEN_BUCKET);

        RateLimiter::TcpServer server(8000, limiter);
        std::cout << "Proxy Server listening on port 8000..." << std::endl;
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}