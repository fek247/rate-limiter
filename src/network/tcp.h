#pragma once
#include "../limiter/rate_limiter.h"
#include <string>

namespace RateLimiter {
    class TcpServer {
        private:
            int server_fd_;
            int port_;
            IRateLimiter& rate_limiter_;

            void handleClient(int client_fd, const std::string& client_ip);
        public:
            TcpServer(int port, IRateLimiter& rate_limiter);
            ~TcpServer();
            void start();
    };
}