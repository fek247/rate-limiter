#include <iostream>
#include <string>
#include <sw/redis++/redis++.h>
#include "parser/parser.h"
#include "limiter/rate_limiter.h"
#include "network/tcp_server.h"
#include "model/algorithm.h"
#include "limiter/rate_limiter_factory.h"

using namespace RateLimiter;

int main(int argc, char* argv[]) {
    std::string config_file_path = (argc > 1) ? argv[1] : "config.yaml";

    try {
        auto config = parse_config(config_file_path);

        std::unique_ptr<IRateLimiter> limiter = RateLimiterFactory::create(config.algorithm, "tcp://127.0.0.1:6379", config);
        if (!limiter) {
            std::cerr << "Error: Unsupported or invalid rate limiter algorithm specified in config." << std::endl;
            return 1;
        }

        TcpServer server(8000, std::move(limiter));
        std::cout << "Proxy Server listening on port 8000..." << std::endl;
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}