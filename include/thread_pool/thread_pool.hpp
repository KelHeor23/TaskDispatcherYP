#pragma once

#include "queue/priority_queue.hpp"
#include <cstddef>
#include <thread>

namespace dispatcher::thread_pool {

class ThreadPool {
private:
    std::shared_ptr<queue::PriorityQueue> priority_queue_;
    std::vector<std::jthread> threads_;

private:
    void run();

public:
    ThreadPool(std::shared_ptr<queue::PriorityQueue> queue, size_t cntThreads);
    ~ThreadPool();

    ThreadPool() = delete;
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
};

}  // namespace dispatcher::thread_pool
