#pragma once

#include <memory>
#include <mutex>

#include "queue/priority_queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"

namespace dispatcher {

class TaskDispatcher {
private:
    std::shared_ptr<queue::PriorityQueue> priority_queue_;
    std::unique_ptr<thread_pool::ThreadPool> thread_pool_;

public:
    TaskDispatcher(size_t thread_count, const std::unordered_map<TaskPriority, queue::QueueOptions> &params = {
                                            {TaskPriority::High, {true, 1000}}, {TaskPriority::Normal, {false}}});

    void schedule(TaskPriority priority, std::function<void()> task);
    ~TaskDispatcher();
};

}  // namespace dispatcher