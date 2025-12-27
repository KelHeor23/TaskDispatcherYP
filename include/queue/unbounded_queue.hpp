#pragma once
#include "queue/queue.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace dispatcher::queue {

class UnboundedQueue : public IQueue {
    std::mutex mtx_;
    std::queue<std::function<void()>> queue_;

public:
    explicit UnboundedQueue();

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~UnboundedQueue() override = default;
};

}  // namespace dispatcher::queue