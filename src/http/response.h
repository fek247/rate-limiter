#include <string>
#include <unistd.h>

namespace RateLimiter {
    class Response {
        public:
            Response(int client_fd) : client_fd_(client_fd) {};
            ~Response();
            static std::string ok();
            static std::string tooManyRequests();
        private:
            int client_fd_;
    };
}