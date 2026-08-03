#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../http/response.h"
#include "tcp.h"

namespace RateLimiter {
    void TcpServer::start() {
        int proxyServerFd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;
        bind(proxyServerFd, (sockaddr *)&addr, sizeof(addr));
        listen(proxyServerFd, 1000);

        while (true) {
            sockaddr_in clientAddr;
            socklen_t size = sizeof(clientAddr);
            int proxyClientFd = accept(proxyServerFd, (sockaddr*)&clientAddr, &size);
            if (proxyClientFd > 0) {
                char* ip = inet_ntoa(clientAddr.sin_addr);
                handleClient(proxyClientFd, ip);
            }
        }
    }

    void TcpServer::handleClient(int client_fd, const std::string& client_ip) {
        char buffer[1024];
        std::string response_str;
        if (rate_limiter_.allowRequest(client_ip)) {
            response_str = Response::ok();
        } else {
            response_str = Response::tooManyRequests();
        }

        write(client_fd, response_str.c_str(), response_str.length());
        close(client_fd);
    }

}