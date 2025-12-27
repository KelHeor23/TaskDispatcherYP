#pragma once
#include "queue/queue.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace dispatcher::queue {

class BoundedQueue : public IQueue {
private:
    std::mutex mtx_;
    std::condition_variable cv_not_full_;
    std::queue<std::function<void()>> queue_;
    std::atomic_bool active_ = true;
    std::atomic_uint capacity_;

public:
    explicit BoundedQueue(int capacity);

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~BoundedQueue() override;
};

}  // namespace dispatcher::queue