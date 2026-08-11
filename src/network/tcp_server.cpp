#include "tcp_server.h"

namespace RateLimiter {
    void TcpServer::start() {
        int num_cores = std::thread::hardware_concurrency();
        thread_pool_ = std::make_unique<ThreadPool>(num_cores - 1);
        for (int i = 0; i < num_cores; i++) {
            thread_pool_->enqueue([this] {
                workerLoop();
            });
        }
    }

    void TcpServer::workerLoop() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            perror("Welcome socket failed");
            return;
        }

        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt failed");
            return;
        }

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(server_fd, (sockaddr *)&addr, sizeof(addr))) {
            perror("Bind failed");
            return;
        }

        if (listen(server_fd, SOMAXCONN) < 0) {
            perror("Listen socket failed");
        }

        setNonBlocking(server_fd);
        int epfd = epoll_create1(0);
        int MAX_EVENT = 10;
        struct epoll_event event, events[MAX_EVENT];
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = server_fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
            perror("epoll_ctl failed");
            return;
        }

        // Add stop fd
        int stop_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        safe_insert(stop_fd);
        struct epoll_event stop_event;
        stop_event.events = EPOLLIN;
        stop_event.data.fd = stop_fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, stop_fd, &stop_event) < 0) {
            perror("epoll ctl stop fd failed");
        }

        while (running_) {
            int num_ready = epoll_wait(epfd, events, MAX_EVENT, -1);

            for (int i = 0; i < num_ready; i++) {
                if (events[i].data.fd == server_fd) {
                    while (true) {
                        sockaddr_in client_addr;
                        socklen_t size = sizeof(client_addr);
                        int connect_fd = accept(server_fd, (sockaddr *)&client_addr, &size);

                        if (connect_fd == -1) {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                break;
                            } else {
                                perror("accept failed");
                                break;
                            }
                        }

                        setNonBlocking(connect_fd);
                    
                        struct epoll_event client_event;
                        client_event.events = EPOLLIN | EPOLLET;
                        ConnectionContext* context = new ConnectionContext();
                        context->fd = connect_fd;
                        inet_ntop(AF_INET, &(client_addr.sin_addr), context->ip, INET_ADDRSTRLEN);
                        client_event.data.ptr = context;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, connect_fd, &client_event);
                    }
                } else if (events[i].data.fd == stop_fd) {
                    uint64_t val;
                    read(stop_fd, &val, sizeof(val));
                } else {
                    ConnectionContext* context = static_cast<ConnectionContext*>(events[i].data.ptr);
                    handleClient(context->fd, context->ip);
                }
            }
        }

        close(stop_fd);
        close(server_fd);
        close(epfd);
    }

    bool TcpServer::setNonBlocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            perror("fcntl F_GETFL");
            return false;
        }

        flags |= O_NONBLOCK;

        if (fcntl(fd, F_SETFL, flags) == -1) {
            perror("fnctl F_SETFL");
            return false;
        }

        return true;
    }

    void TcpServer::handleClient(int client_fd, const std::string& client_ip) {
        char buffer[1024];
        int byteReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (byteReceived > 0) {
            std::string response_str;
            if (rate_limiter_->allowRequest(client_ip)) {
                response_str = Response::ok();
            } else {
                response_str = Response::tooManyRequests();
            }

            write(client_fd, response_str.c_str(), response_str.length());
        }
        
        close(client_fd);
    }

    void TcpServer::stop() {
        if (!running_) {
            return;
        }

        running_ = false;

        uint64_t u = 1;
        for (int stop_fd : stop_fds_) {
            write(stop_fd, &u, sizeof(u));
        }

        if (thread_pool_) {
            thread_pool_.reset();
        }
    }

    void TcpServer::safe_insert(int val) {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_fds_.push_back(val);
    }
}