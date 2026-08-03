#include "response.h"

namespace RateLimiter {
    std::string Response::ok() {
        return
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 2\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "OK";
    }

    std::string Response::tooManyRequests() {
        return
            "HTTP/1.1 429 Too Many Requests\r\n"
            "Content-Length: 18\r\n"
            "Content-Type: text/plain\r\n"
            "Retry-After: 1\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Too Many Requests";
    }
}