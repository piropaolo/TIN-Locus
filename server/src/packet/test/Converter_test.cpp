#include "gtest/gtest.h"
#include "../Converter.h"

using namespace packet;

TEST(Converter_toBytes, ConvertFromUInt8) {
    uint8_t test = 10;
    for (int i = 0; i < sizeof test; ++i) {
        EXPECT_EQ(to_bytes(test).at(i), static_cast<std::byte>((test >> i * 8) & 0xFF));
    }
}

TEST(Converter_toBytes, ConvertFromInt) {
    int test = 10;
    for (int i = 0; i < sizeof test; ++i) {
        EXPECT_EQ(to_bytes(test).at(i), static_cast<std::byte>((test >> i * 8) & 0xFF));
    }
}

TEST(Converter_toBytes, ConvertFromLong) {
    long test = 10;
    for (int i = 0; i < sizeof test; ++i) {
        EXPECT_EQ(to_bytes(test).at(i), static_cast<std::byte>((test >> i * 8) & 0xFF));
    }
}

TEST(Converter_toBytes, ConvertFromLongLong) {
    long long test = 10;
    for (int i = 0; i < sizeof test; ++i) {
        EXPECT_EQ(to_bytes(test).at(i), static_cast<std::byte>((test >> i * 8) & 0xFF));
    }
}



TEST(Converter_toBytes, ConvertToUInt8) {
    uint8_t test = 10;
    EXPECT_EQ(from_bytes(to_bytes(test),test), test);
}

TEST(Converter_toBytes, ConvertToInt) {
    int test = 10;
    EXPECT_EQ(from_bytes(to_bytes(test),test), test);
}

TEST(Converter_toBytes, ConvertToLong) {
    long test = 10;
    EXPECT_EQ(from_bytes(to_bytes(test),test), test);
}

TEST(Converter_toBytes, ConvertToLongLong) {
    long long test = 10;
    EXPECT_EQ(from_bytes(to_bytes(test),test), test);
}