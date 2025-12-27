#include "queue/priority_queue.hpp"
#include "queue/bounded_queue.hpp"
#include "queue/unbounded_queue.hpp"
#include <memory>
#include <mutex>
#include <optional>

namespace dispatcher::queue {

PriorityQueue::PriorityQueue(const std::unordered_map<TaskPriority, QueueOptions> &um_options) {
    for (const auto &[priority, options] : um_options) {
        if (options.bounded) {
            if (options.capacity.has_value())
                priority_queues_[priority] = std::make_unique<BoundedQueue>(options.capacity.value());
            else
                throw std::invalid_argument("Capacity must be initiated");
        } else {
            priority_queues_[priority] = std::make_unique<UnboundedQueue>();
        }
    }
}

void PriorityQueue::push(TaskPriority priority, std::function<void()> task) {
    auto it = priority_queues_.find(priority);
    if (it == priority_queues_.end())
        throw std::invalid_argument("Queue with this priority is not exist");
    // Синхронизация push и pop у каждой очереди своя
    // В bounded блокирующий push, если добавить не поулчилось ждем
    it->second->push(std::move(task));
    // После добавления увеличиваем счетчик
    {
        std::lock_guard lock(mtx_);
        cnt_tasks_++;
        cv_task_exist_.notify_one();
    }
}

std::optional<std::function<void()>> PriorityQueue::pop() {
    std::unique_lock lock(mtx_);

    cv_task_exist_.wait(lock, [this] { return !active_ || cnt_tasks_ > 0; });

    if (cnt_tasks_ > 0) {
        for (const auto &priority : AllTaskPriorities) {
            auto task = priority_queues_.find(priority);
            if (task != priority_queues_.end()) {
                auto res = task->second->try_pop();
                if (res.has_value()) {
                    cnt_tasks_--;
                    return res;
                }
            }
        }
    }

    return std::nullopt;
}

void PriorityQueue::shutdown() {
    active_ = false;
    cv_task_exist_.notify_all();
}

PriorityQueue::~PriorityQueue() { shutdown(); }
}  // namespace dispatcher::queue