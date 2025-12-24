#include "queue/bounded_queue.hpp"
#include <memory>
#include <mutex>
#include <optional>

namespace dispatcher::queue {

BoundedQueue::BoundedQueue(int capacity) {
    if (capacity <= 0)
        throw std::invalid_argument("Capacity must be positive");
    option_ = std::make_unique<QueueOptions>(true, capacity);
}

void BoundedQueue::push(std::function<void()> task) {
    std::unique_lock lock(mtx_);

    cv_not_full_.wait(lock, [this] { return queue_.size() < option_->capacity; });

    queue_.push(std::move(task));
    cv_not_empty_.notify_one();
}

std::optional<std::function<void()>> BoundedQueue::try_pop() {
    std::unique_lock lock(mtx_);

    // cv_not_empty_.wait(lock, [this] { return !queue_.empty(); });

    if (queue_.empty())
        return std::nullopt;

    auto res = std::move(queue_.front());
    queue_.pop();

    cv_not_full_.notify_one();
    return res;
}

}  // namespace dispatcher::queue