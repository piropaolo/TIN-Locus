#include "gtest/gtest.h"
#include "../PacketType.h"

using namespace packet;

TEST(PacketType, toUint8MethodWithOPEN) {
    EXPECT_EQ(PacketType::toUint8(PacketType::OPEN), 0);
}
TEST(PacketType, toUint8MethodWithCLOSE) {
    EXPECT_EQ(PacketType::toUint8(PacketType::CLOSE), 1);
}
TEST(PacketType, toUint8MethodWithACKOK) {
    EXPECT_EQ(PacketType::toUint8(PacketType::ACK_OK), 2);
}
TEST(PacketType, toUint8MethodWithACKERR) {
    EXPECT_EQ(PacketType::toUint8(PacketType::ACK_ERR), 3);
}
TEST(PacketType, toUint8MethodWithALIVE) {
    EXPECT_EQ(PacketType::toUint8(PacketType::ALIVE), 4);
}


TEST(PacketType, toUint8MethodWithOPENENCR) {
    EXPECT_EQ(PacketType::toUint8(PacketType::OPEN_ENCR), 8);
}
TEST(PacketType, toUint8MethodWithPUBLICKEY) {
    EXPECT_EQ(PacketType::toUint8(PacketType::PUBLIC_KEY), 9);
}
TEST(PacketType, toUint8MethodWithSYMMETRICKEY) {
    EXPECT_EQ(PacketType::toUint8(PacketType::SYMMETRIC_KEY), 10);
}
TEST(PacketType, toUint8MethodWithTESTKEY) {
    EXPECT_EQ(PacketType::toUint8(PacketType::TEST_KEY), 11);
}


TEST(PacketType, toUint8MethodWithOPENPROT) {
    EXPECT_EQ(PacketType::toUint8(PacketType::OPEN_PROT), 16);
}
TEST(PacketType, toUint8MethodWithSETNAME) {
    EXPECT_EQ(PacketType::toUint8(PacketType::SET_NAME), 17);
}
TEST(PacketType, toUint8MethodWithADDFOLLOWER) {
    EXPECT_EQ(PacketType::toUint8(PacketType::ADD_FOLLOWER), 18);
}
TEST(PacketType, toUint8MethodWithNEWFOLLOWED) {
    EXPECT_EQ(PacketType::toUint8(PacketType::NEW_FOLLOWED), 19);
}
TEST(PacketType, toUint8MethodWithREMOVEFOLLOWER) {
    EXPECT_EQ(PacketType::toUint8(PacketType::REMOVE_FOLLOWER), 20);
}
TEST(PacketType, toUint8MethodWithREMOVEFOLLOWED) {
    EXPECT_EQ(PacketType::toUint8(PacketType::REMOVE_FOLLOWED), 21);
}
TEST(PacketType, toUint8MethodWithMYLOCATION) {
    EXPECT_EQ(PacketType::toUint8(PacketType::MY_LOCATION), 22);
}
TEST(PacketType, toUint8MethodWithLOCATION) {
    EXPECT_EQ(PacketType::toUint8(PacketType::LOCATION), 23);
}



TEST(PacketType, toPackeTypeMethodWithOPEN) {
    EXPECT_EQ(PacketType::toPackeType(0), PacketType::OPEN);
}
TEST(PacketType, toPackeTypeMethodWithCLOSE) {
    EXPECT_EQ(PacketType::toPackeType(1), PacketType::CLOSE);
}
TEST(PacketType, toPackeTypeMethodWithACKOK) {
    EXPECT_EQ(PacketType::toPackeType(2), PacketType::ACK_OK);
}
TEST(PacketType, toPackeTypeMethodWithACKERR) {
    EXPECT_EQ(PacketType::toPackeType(3), PacketType::ACK_ERR);
}
TEST(PacketType, toPackeTypeMethodWithALIVE) {
    EXPECT_EQ(PacketType::toPackeType(4), PacketType::ALIVE);
}


TEST(PacketType, toPackeTypeMethodWithOPENENCR) {
    EXPECT_EQ(PacketType::toPackeType(8), PacketType::OPEN_ENCR);
}
TEST(PacketType, toPackeTypeMethodWithPUBLICKEY) {
    EXPECT_EQ(PacketType::toPackeType(9), PacketType::PUBLIC_KEY);
}
TEST(PacketType, toPackeTypeMethodWithSYMMETRICKEY) {
    EXPECT_EQ(PacketType::toPackeType(10), PacketType::SYMMETRIC_KEY);
}
TEST(PacketType, toPackeTypeMethodWithTESTKEY) {
    EXPECT_EQ(PacketType::toPackeType(11), PacketType::TEST_KEY);
}


TEST(PacketType, toPackeTypeMethodWithOPENPROT) {
    EXPECT_EQ(PacketType::toPackeType(16), PacketType::OPEN_PROT);
}
TEST(PacketType, toPackeTypeMethodWithSETNAME) {
    EXPECT_EQ(PacketType::toPackeType(17), PacketType::SET_NAME);
}
TEST(PacketType, toPackeTypeMethodWithADDFOLLOWER) {
    EXPECT_EQ(PacketType::toPackeType(18), PacketType::ADD_FOLLOWER);
}
TEST(PacketType, toPackeTypeMethodWithNEWFOLLOWED) {
    EXPECT_EQ(PacketType::toPackeType(19), PacketType::NEW_FOLLOWED);
}
TEST(PacketType, toPackeTypeMethodWithREMOVEFOLLOWER) {
    EXPECT_EQ(PacketType::toPackeType(20), PacketType::REMOVE_FOLLOWER);
}
TEST(PacketType, toPackeTypeMethodWithREMOVEFOLLOWED) {
    EXPECT_EQ(PacketType::toPackeType(21), PacketType::REMOVE_FOLLOWED);
}
TEST(PacketType, toPackeTypeMethodWithMYLOCATION) {
    EXPECT_EQ(PacketType::toPackeType(22), PacketType::MY_LOCATION);
}
TEST(PacketType, toPackeTypeMethodWithLOCATION) {
    EXPECT_EQ(PacketType::toPackeType(23), PacketType::LOCATION);
}