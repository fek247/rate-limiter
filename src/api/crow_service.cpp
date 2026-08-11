#include "crow_service.h"

namespace RateLimiter {
    void CrowService::run() {
        CROW_ROUTE(app_, "/")([](){
            return "OK";
        });

        app_.signal_clear();

        use_multi_thread_ ? app_.port(port_).multithreaded().run()
            : app_.port(port_).run();
    }

    void CrowService::stop() {
        app_.stop();
    }
}