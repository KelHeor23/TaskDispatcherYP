#include "task_dispatcher.hpp"
#include <memory>

namespace dispatcher {

TaskDispatcher::TaskDispatcher(size_t thread_count,
                               const std::unordered_map<TaskPriority, queue::QueueOptions> &params) {
    priority_queue_ = std::make_shared<queue::PriorityQueue>(params);
    thread_pool_ = std::make_unique<thread_pool::ThreadPool>(priority_queue_, thread_count);
}

void TaskDispatcher::schedule(TaskPriority priority, std::function<void()> task) {
    priority_queue_->push(priority, task);
}

TaskDispatcher::~TaskDispatcher() { priority_queue_->shutdown(); }
}  // namespace dispatcher