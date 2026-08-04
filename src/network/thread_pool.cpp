#include "thread_pool.h"

namespace RateLimiter {
    ThreadPool::ThreadPool(size_t num_threads) : _stop(false) {
        for (int i = 0; i < num_threads; i++) {
            _threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(_queue_mutex);

                        _condition.wait(lock, [this] {
                            return this->_stop || !this->_tasks.empty();
                        });

                        if (this->_stop && this->_tasks.empty()) {
                            return;
                        }

                        task = std::move(_tasks.front());
                        _tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    ThreadPool::~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _stop = true;
        }
        _condition.notify_all();

        for (std::thread &thread : _threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void ThreadPool::enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _tasks.push(std::move(task));
        }

        _condition.notify_one();
    }
}