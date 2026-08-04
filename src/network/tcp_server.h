#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sys/epoll.h>
#include <fcntl.h>
#include "http/response.h"
#include "limiter/rate_limiter.h"
#include "thread_pool.h"

namespace RateLimiter {
    struct ConnectionContext {
        int fd;
        char ip[INET_ADDRSTRLEN];
    };

    class TcpServer {
        private:
            int server_fd_;
            int port_;
            IRateLimiter& rate_limiter_;

            void handleClient(int client_fd, const std::string& client_ip);
            bool setNonBlocking(int fd);
        public:
            TcpServer(int port, IRateLimiter& limiter) : port_(port), rate_limiter_(limiter) {};
            ~TcpServer() {};
            void start();
            void workerLoop();
    };
}