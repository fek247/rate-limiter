#pragma once
#include <crow.h>
#include <iostream>

namespace RateLimiter {
    class CrowService {
        private:
            int port_;
            bool use_multi_thread_;
            crow::SimpleApp app_;
        public:
            CrowService(int port = 80, bool use_multi_thread = false) 
                : port_(port), use_multi_thread_(use_multi_thread) {}
            void run();
            void stop();
    };
}
