#include "gtest/gtest.h"
#include "../BlockingQueue.h"

#include <chrono>

using namespace message;
using namespace std::chrono_literals;

TEST(Constructor, EmptyQueue) {
    BlockingQueue<int> queue(2);
    EXPECT_TRUE(queue.empty());
    ASSERT_EQ(queue.size(), 0);
}

TEST(Push, toEmptyQueueOneElem) {
    BlockingQueue<int> queue(2);
    queue.push(1);
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1);
}

TEST(Push, toEmptyQueueTwoElem) {
    BlockingQueue<int> queue(2);
    queue.push(1);
    queue.push(2);
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 2);
}

TEST(Pop, fromNonEmptyQueue) {
    BlockingQueue<int> queue(2);
    queue.push(1);
    EXPECT_EQ(queue.pop(), 1);
    EXPECT_TRUE(queue.empty());
}

TEST(Pop, fromNonEmptyQueueWthTwoElem) {
    BlockingQueue<int> queue(2);
    queue.push(1);
    queue.push(2);
    EXPECT_EQ(queue.pop(), 1);
    EXPECT_EQ(queue.pop(), 2);
    EXPECT_TRUE(queue.empty());
}

TEST(PriorityPush, toEmptyQueueOneElem) {
    BlockingQueue<int> queue(2);
    queue.priorityPush(2);

    ASSERT_EQ(queue.size(), 1);
    EXPECT_EQ(queue.pop(), 2);
    EXPECT_TRUE(queue.empty());
}

TEST(PriorityPush, toEmptyQueueTwoElem) {
    BlockingQueue<int> queue(2);
    queue.push(1);
    queue.priorityPush(2);

    ASSERT_EQ(queue.size(), 2);
    EXPECT_EQ(queue.pop(), 2);
    EXPECT_EQ(queue.pop(), 1);
    EXPECT_TRUE(queue.empty());
}

TEST(PopFor, fromEmptyQueue) {
    BlockingQueue<int> queue(2);
    queue.pop_for(1ms);
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
}

TEST(PopFor, fromNonEmptyQueue) {
    BlockingQueue<int> queue(2);
    queue.push(1);
    EXPECT_EQ(queue.pop_for(1ms), 1);
    EXPECT_TRUE(queue.empty());
}

TEST(PopFor, fromNonEmptyQueueWthTwoElem) {
    BlockingQueue<int> queue(2);
    queue.push(1);
    queue.push(2);
    EXPECT_EQ(queue.pop_for(1ms), 1);
    EXPECT_EQ(queue.pop_for(1ms), 2);
    EXPECT_TRUE(queue.empty());
}

