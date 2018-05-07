#include "gtest/gtest.h"
#include "../BlockBuffer.h"

#include "../Converter.h"

using namespace packet;

TEST(getSize, Empty) {
    BlockBuffer buffer;
    EXPECT_EQ(buffer.size(), 0);
}

TEST(push, fewElem) {
    BlockBuffer buffer;

    std::vector<std::byte> bytes(5, std::byte(1));
    buffer.push(bytes);
    EXPECT_EQ(buffer.size(), 1);

    buffer.push(std::vector<std::byte>(5, std::byte(2)));
    EXPECT_EQ(buffer.size(), 2);

    buffer.push(2);
    EXPECT_EQ(buffer.size(), 3);
}

TEST(pop, Empty) {
    BlockBuffer buffer;

    ASSERT_THROW(buffer.pop(), std::runtime_error);

    int x;
    ASSERT_THROW(buffer.pop(x), std::runtime_error);
}

TEST(pop, NonEmpty1) {
    BlockBuffer buffer;

    std::vector<std::byte> bytes(5, std::byte(1));
    std::vector<std::byte> bytes2(bytes);
    buffer.push(bytes);

    EXPECT_EQ(buffer.pop(), bytes2);
    EXPECT_EQ(buffer.size(), 0);
}

TEST(pop, NonEmpty2) {
    BlockBuffer buffer;

    std::vector<std::byte> bytes(5, std::byte(1));
    std::vector<std::byte> bytes2(bytes);
    buffer.push(std::move(bytes));

    auto out = buffer.pop();
    EXPECT_NE(out, bytes);
    EXPECT_EQ(out, bytes2);
    EXPECT_EQ(buffer.size(), 0);
}

TEST(pop, NonEmpty3) {
    BlockBuffer buffer;

    int x;
    int pre = x;
    buffer.push(to_bytes(x));

    auto post = buffer.pop();
    EXPECT_EQ(post, to_bytes(pre));
    EXPECT_EQ(buffer.size(), 0);
}

TEST(pop, NonEmpty4) {
    BlockBuffer buffer;

    std::vector<std::byte> bytes(4, std::byte(0));
    buffer.push(bytes);

    int result;
    EXPECT_EQ(buffer.pop(result), 0);
    EXPECT_EQ(buffer.size(), 0);
}

TEST(pop, NonEmpty5) {
    BlockBuffer buffer;

    std::vector<std::byte> bytes(4, std::byte(0));
    buffer.push(std::move(bytes));

    int result;
    EXPECT_EQ(buffer.pop(result), 0);
    EXPECT_EQ(buffer.size(), 0);
}

TEST(pop, NonEmpty6) {
    BlockBuffer buffer;

    int x;
    int pre = x;
    int post;
    buffer.push(x);

    EXPECT_EQ(buffer.pop(post), pre);
    EXPECT_EQ(buffer.size(), 0);
}