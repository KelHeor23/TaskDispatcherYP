#include <gtest/gtest.h>
#include <thread>

#include "queue/bounded_queue.hpp"

namespace dispatcher::queue {
TEST(BoundedQueue, ConstructorValid) {
    EXPECT_NO_THROW({ BoundedQueue q(1); });
}
TEST(BoundedQueue, ConstructorInvalid) {
    EXPECT_THROW({ BoundedQueue q(0); }, std::invalid_argument);
}

TEST(BoundedQueue, TryPopSingle) {
    BoundedQueue queue(1);

    bool temp = false;

    queue.push([&temp]() { temp = true; });

    auto task = queue.try_pop();
    task.value()();

    EXPECT_TRUE(temp);
}

TEST(BoundedQueue, TryPopMany) {
    BoundedQueue queue(1);

    std::atomic<bool> temp1{false}, temp2{false}, temp3{false};

    std::thread t1{[&queue, &temp1] { queue.push([&temp1]() { temp1 = true; }); }};
    std::thread t2{[&queue, &temp2] { queue.push([&temp2]() { temp2 = true; }); }};
    std::thread t3{[&queue, &temp3] { queue.push([&temp3]() { temp3 = true; }); }};

    t1.detach();
    t2.detach();
    t3.detach();

    /*for (int i = 0; i < 3; i++) {
        auto task = queue.try_pop();
        task.value()();
    }*/

    for (int i = 0; i < 3;) {
        auto task = queue.try_pop();
        if (task.has_value()) {
            task.value()();
            i++;
        }
    }

    EXPECT_TRUE(temp1);
    EXPECT_TRUE(temp2);
    EXPECT_TRUE(temp3);
}

TEST(BoundedQueue, FIFOOrder) {
    BoundedQueue queue(5);

    std::vector<int> execution_order;

    for (int i = 0; i < 5; i++) {
        queue.push([&execution_order, i]() { execution_order.push_back(i); });
    }

    for (int i = 0; i < 5; i++) {
        auto task = queue.try_pop();
        ASSERT_TRUE(task.has_value());
        (*task)();
    }

    EXPECT_EQ(execution_order, std::vector<int>({0, 1, 2, 3, 4}));
}

}  // namespace dispatcher::queue

// здесь ваш код