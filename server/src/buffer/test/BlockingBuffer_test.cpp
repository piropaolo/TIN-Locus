#include "gtest/gtest.h"
#include "buffer/Buffer.h"


TEST(Buffer_Constructor, Initialize) {
    Buffer buffer;
    EXPECT_EQ(1, 1);
}

TEST(Buffer_Pop, popPart) {
    Buffer buffer;
    const auto byte = std::byte(0);
    buffer.push_back(std::vector<std::byte>(2, byte));
    EXPECT_EQ(buffer.size(), 2);

    EXPECT_EQ(buffer.pop(1), std::vector<std::byte>(1, byte));
    EXPECT_EQ(buffer.size(), 1);

    EXPECT_EQ(buffer.pop(1), std::vector<std::byte>(1, byte));
    EXPECT_EQ(buffer.size(), 0);
}

TEST(Buffer_Pop, popExactly) {
    Buffer buffer;
    const auto byte = std::byte(0);
    buffer.push_back(std::vector<std::byte>(2, byte));

    EXPECT_EQ(buffer.pop(2), std::vector<std::byte>(2, byte));
    EXPECT_EQ(buffer.size(), 0);
}

TEST(Buffer_Pop, popCrossPart) {
    Buffer buffer;
    const auto byte = std::byte(0);
    buffer.push_back(std::vector<std::byte>(2, byte));
    buffer.push_back(std::vector<std::byte>(2, byte));

    EXPECT_EQ(buffer.pop(3), std::vector<std::byte>(3, byte));
    EXPECT_EQ(buffer.size(), 1);

    EXPECT_EQ(buffer.pop(1), std::vector<std::byte>(1, byte));
    EXPECT_EQ(buffer.size(), 0);
}

TEST(Buffer_Pop, popCrossExactly) {
    Buffer buffer;
    const auto byte = std::byte(0);
    buffer.push_back(std::vector<std::byte>(2, byte));
    buffer.push_back(std::vector<std::byte>(2, byte));

    EXPECT_EQ(buffer.pop(4), std::vector<std::byte>(4, byte));
    EXPECT_EQ(buffer.size(), 0);
}
