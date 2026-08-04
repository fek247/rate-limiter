#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>

namespace RateLimiter {
    class ThreadPool {
        private:
            std::vector<std::thread> _threads;
            std::queue<std::function<void()>> _tasks;
            std::mutex _queue_mutex;
            std::condition_variable _condition;
            bool _stop;

        public:
            ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
            ~ThreadPool();

            ThreadPool(ThreadPool&) = delete;
            ThreadPool(const ThreadPool&) = delete;
            ThreadPool& operator=(ThreadPool&&) = delete;
            ThreadPool& operator=(const ThreadPool&) = delete;

            void enqueue(std::function<void()> task);
    };
}