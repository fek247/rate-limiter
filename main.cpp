#include <sys/socket.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sw/redis++/redis++.h>
#include "parser/parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    // int proxyServerFd = socket(AF_INET, SOCK_STREAM, 0);
    // sockaddr_in addr;
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(8000);
    // addr.sin_addr.s_addr = INADDR_ANY;
    // bind(proxyServerFd, (sockaddr *)&addr, sizeof(addr));
    // listen(proxyServerFd, 1000);
    // std::chrono::seconds ttl(1); 
    // sw::redis::Redis redis("tcp://127.0.0.1:6379");

    // while (true) {
    //     sockaddr_in clientAddr;
    //     socklen_t size = sizeof(clientAddr);
    //     int proxyClientFd = accept(proxyServerFd, (sockaddr*)&clientAddr, &size);
    //     if (proxyClientFd > 0) {
    //         char buffer[1024];
    //         read(proxyClientFd, buffer, sizeof(buffer));

    //         char* ip = inet_ntoa(clientAddr.sin_addr);

    //         std::string lua_script =R"(
    //             local key = KEYS[1]
    //             local max_requests = tonumber(ARGV[1])
    //             local window_seconds = tonumber(ARGV[2])

    //             local count = redis.call('INCR', key)

    //             if count == 1 then
    //                 redis.call('EXPIRE', key, window_seconds)
    //             end

    //             local pttl = redis.call('PTTL', key)

    //             return { count, pttl }
    //         )";
            
    //         int max_requests = 100; 
    //         int windows_seconds = 60;
    //         auto result = redis.command<std::vector<long long>>("EVAL", lua_script, 1, ip, std::to_string(max_requests), std::to_string(windows_seconds));
    //         long long count = result[0];
    //         long long pttl = result[1];
    //         bool allowed = count <= max_requests;
    //         //int remaining = std::max(0, max_requests - count);

    //         if (allowed) {
    //             std::string response = 
    //                 "HTTP/1.1 200 OK\r\n"
    //                 "Content-Length: 2\r\n"
    //                 "Content-Type: text/plain\r\n"
    //                 "Connection: close\r\n"
    //                 "\r\n"
    //                 "OK";
                
    //             write(proxyClientFd, response.c_str(), response.length());
    //         } else {
    //             std::string response = 
    //                 "HTTP/1.1 429 Too Many Requests\r\n"
    //                 "Content-Length: 18\r\n"
    //                 "Content-Type: text/plain\r\n"
    //                 "Retry-After: 1\r\n"
    //                 "Connection: close\r\n"
    //                 "\r\n"
    //                 "Too Many Requests";
                
    //             write(proxyClientFd, response.c_str(), response.length());
    //         }
    //         close(proxyClientFd);
    //     } else {
    //         cout << "Connection connect failed!" << endl;
    //     }
    // }
    std::string config_file_path = "config.yaml";

    if (argc > 1) {
        config_file_path = argv[1];
    }

    try {
        RateLimiter::RateLimitConfig config = RateLimiter::parse_config(config_file_path);
        cout << "limit: " << config.limit << endl;
    } catch (const std::exception& e) {
        std::cerr << "Config error: " << e.what() << std::endl;
    }

    return 0;
}