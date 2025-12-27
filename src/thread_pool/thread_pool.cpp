#include "thread_pool/thread_pool.hpp"

namespace dispatcher::thread_pool {

ThreadPool::ThreadPool(std::shared_ptr<queue::PriorityQueue> queue, size_t cntThreads) : priority_queue_(queue) {
    if (!priority_queue_)
        throw std::invalid_argument("PriorityQueue must be init");

    if (cntThreads == 0)
        throw std::invalid_argument("The number of threads must be greater than 0");

    threads_.reserve(cntThreads);

    for (int i = 0; i < cntThreads; i++) {
        threads_.emplace_back(&ThreadPool::run, this);
    }
}

ThreadPool::~ThreadPool() {
    for (auto &thread : threads_) {
        if (thread.joinable())
            thread.join();
    }
}

void ThreadPool::run() {
    while (true) {
        auto task = priority_queue_->pop();
        if (!task.has_value())
            break;

        task.value()();
    }
}
}  // namespace dispatcher::thread_pool