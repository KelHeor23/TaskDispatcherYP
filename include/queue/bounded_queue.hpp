#pragma once
#include "queue/queue.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace dispatcher::queue {

class BoundedQueue : public IQueue {
private:
    std::unique_ptr<QueueOptions> option_;
    std::mutex mtx_;
    std::condition_variable cv_not_full_;
    std::condition_variable cv_not_empty_;
    std::queue<std::function<void()>> queue_;

public:
    explicit BoundedQueue(int capacity);

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~BoundedQueue() override = default;
};

}  // namespace dispatcher::queue