#include "queue/bounded_queue.hpp"
#include <memory>
#include <mutex>
#include <optional>

namespace dispatcher::queue {

BoundedQueue::BoundedQueue(int capacity) {
    if (capacity <= 0)
        throw std::invalid_argument("Capacity must be positive");
    capacity_ = capacity;
}

void BoundedQueue::push(std::function<void()> task) {
    std::unique_lock lock(mtx_);

    cv_not_full_.wait(lock, [this] { return queue_.size() < capacity_ || !active_; });

    if (!active_)
        return;

    queue_.push(std::move(task));
}

std::optional<std::function<void()>> BoundedQueue::try_pop() {
    std::unique_lock lock(mtx_);

    if (queue_.empty())
        return std::nullopt;

    auto res = std::move(queue_.front());
    queue_.pop();

    cv_not_full_.notify_one();
    return res;
}

BoundedQueue::~BoundedQueue() {
    active_ = false;
    cv_not_full_.notify_all();
}
}  // namespace dispatcher::queue