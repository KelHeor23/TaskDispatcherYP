#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

#include "task_dispatcher.hpp"
#include "types.hpp"

namespace dispatcher {
TEST(TaskDispatcher, DefaultConstructor) {
    EXPECT_NO_THROW({ TaskDispatcher task_dispatcher(5); });
}

TEST(TaskDispatcher, Constructor) {
    EXPECT_NO_THROW(([] {
        std::unordered_map<TaskPriority, queue::QueueOptions> params = {{TaskPriority::High, {true, 1000}},
                                                                        {TaskPriority::Normal, {false}}};
        TaskDispatcher task_dispatcher(2, params);
    })());
}

TEST(TaskDispatcher, ConstructorZeroThrows) {
    EXPECT_THROW({ TaskDispatcher task_dispatcher(0); }, std::invalid_argument);
}

TEST(TaskDispatcher, ExecTask) {
    TaskDispatcher task_dispatcher(2);

    std::atomic_bool some_flag = false;

    task_dispatcher.schedule(TaskPriority::Normal, [&some_flag] { some_flag = true; });

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    EXPECT_TRUE(some_flag.load());
}

TEST(TaskDispatcher, ExecTasksByOrder) {
    TaskDispatcher task_dispatcher(1);

    std::atomic_bool some_flag = false;

    task_dispatcher.schedule(TaskPriority::Normal,
                             [&some_flag] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
    task_dispatcher.schedule(TaskPriority::Normal,
                             [&some_flag] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
    task_dispatcher.schedule(TaskPriority::High, [&some_flag] { some_flag = true; });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    EXPECT_TRUE(some_flag.load());
}
}  // namespace dispatcher
// здесь ваш код