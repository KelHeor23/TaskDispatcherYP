#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <condition_variable>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace dispatcher::queue {

class PriorityQueue {
    std::atomic_bool active_ = true;
    std::mutex mtx_;
    std::condition_variable cv_task_exist_;
    std::unordered_map<TaskPriority, std::unique_ptr<IQueue>> priority_queues_;
    std::atomic_uint cnt_tasks_ = 0;

public:
    explicit PriorityQueue(const std::unordered_map<TaskPriority, QueueOptions> &);

    void push(TaskPriority priority, std::function<void()> task);
    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<std::function<void()>> pop();

    void shutdown();

    ~PriorityQueue();
};

}  // namespace dispatcher::queue