#include <future>
#include <gtest/gtest.h>
#include <thread>

#include "queue/priority_queue.hpp"
#include "types.hpp"

namespace dispatcher::queue {
TEST(PriorityQueue, ConstructorValid) {
    EXPECT_NO_THROW(([] {
        std::unordered_map<TaskPriority, QueueOptions> config = {{TaskPriority::High, {true, 5}},
                                                                 {TaskPriority::Normal, {false}}};
        PriorityQueue priority_queue(config);
    })());
}

TEST(PriorityQueue, ConstructorInvalid) {
    EXPECT_THROW(([] {
                     std::unordered_map<TaskPriority, QueueOptions> config = {{TaskPriority::High, {true, 0}},
                                                                              {TaskPriority::Normal, {false}}};
                     PriorityQueue priority_queue(config);
                 })(),
                 std::invalid_argument);
}

TEST(PriorityQueue, TryPopInvalid) {
    std::unordered_map<TaskPriority, QueueOptions> config = {{TaskPriority::High, {true, 1}}};
    PriorityQueue priority_queue(config);

    EXPECT_THROW(priority_queue.push(TaskPriority::Normal, []() {}), std::invalid_argument);
}

TEST(PriorityQueue, TryPopByPriorityOrder) {
    uint cntHight = 0;
    uint cntNormal = 0;
    std::unordered_map<TaskPriority, QueueOptions> config = {{TaskPriority::High, {true, 5}},
                                                             {TaskPriority::Normal, {false}}};
    PriorityQueue priority_queue(config);

    for (int i = 0; i < 5; i++) {
        priority_queue.push(TaskPriority::Normal, [&cntNormal]() { cntNormal++; });
    }

    for (int i = 0; i < 5; i++) {
        priority_queue.push(TaskPriority::High, [&cntHight]() { cntHight++; });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    for (int i = 0; i < 5; i++) {
        auto task = priority_queue.pop();
        if (task.has_value())
            task.value()();
    }

    EXPECT_EQ(cntHight, 5);
    EXPECT_EQ(cntNormal, 0);

    for (int i = 0; i < 5; i++) {
        auto task = priority_queue.pop();
        if (task.has_value())
            task.value()();
    }

    EXPECT_EQ(cntHight, 5);
    EXPECT_EQ(cntNormal, 5);
}

TEST(PriorityQueue, ShutdownWhenPopBlocked) {
    std::unordered_map<TaskPriority, QueueOptions> config = {{TaskPriority::High, {true, 1}}};
    PriorityQueue priority_queue(config);

    // Использую future, потмоу что у него можно поймать статус завершения
    auto future = std::async(std::launch::async, [&priority_queue] { return priority_queue.pop().has_value(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    priority_queue.shutdown();

    EXPECT_EQ(future.wait_for(std::chrono::milliseconds(20)), std::future_status::ready);
}
}  // namespace dispatcher::queue