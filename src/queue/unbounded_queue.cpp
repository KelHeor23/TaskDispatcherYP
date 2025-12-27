#include "queue/unbounded_queue.hpp"

#include <functional>
#include <mutex>

namespace dispatcher::queue {

UnboundedQueue::UnboundedQueue() {}

void UnboundedQueue::push(std::function<void()> task) {
    std::unique_lock lock(mtx_);
    queue_.push(std::move(task));
}

std::optional<std::function<void()>> UnboundedQueue::try_pop() {
    std::unique_lock lock(mtx_);

    if (queue_.empty())
        return std::nullopt;

    auto res = std::move(queue_.front());
    queue_.pop();
    return res;
}

}  // namespace dispatcher::queue