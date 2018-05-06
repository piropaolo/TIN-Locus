#include "gtest/gtest.h"
#include "../Packet.h"

#include "../Converter.h"

using namespace packet;

TEST(getType, Open) {
    Packet packet(PacketType::OPEN);
    EXPECT_EQ(packet.getType(), PacketType::OPEN);
}

TEST(getType, Close) {
    Packet packet(PacketType::CLOSE);
    EXPECT_EQ(packet.getType(), PacketType::CLOSE);
}

TEST(getType, Alive) {
    Packet packet(PacketType::ALIVE);
    EXPECT_EQ(packet.getType(), PacketType::ALIVE);
}

TEST(getTypeSize, Open) {
    Packet packet(PacketType::OPEN);
    EXPECT_EQ(packet.getTypeSize(), 1 * 8);
}

TEST(getBuffer, empty) {
    Packet packet(PacketType::OPEN);
    EXPECT_TRUE(packet.getBuffer().empty());
}

TEST(getBuffer, notEmpty) {
    Packet packet(PacketType::OPEN);
    packet.setBuffer(to_bytes(int(0)));
    EXPECT_FALSE(packet.getBuffer().empty());
}

TEST(getBuffer, notEmpty2) {
    Packet packet(PacketType::OPEN);
    packet.pushToBuffer(to_bytes(int(0)));
    EXPECT_FALSE(packet.getBuffer().empty());
}

TEST(getSize, SetName) {
    Packet packet(PacketType::SET_NAME);
    packet.pushToBuffer(to_bytes(uint8_t(3)));
    EXPECT_EQ(packet.getTypeSize(), sizeof(uint8_t) * 8);

    packet.pushToBuffer(to_bytes('a'));
    EXPECT_EQ(packet.getBufferSize(), (sizeof(uint8_t) + 1 * sizeof(char)) * 8);
    packet.pushToBuffer(to_bytes('l'));
    EXPECT_EQ(packet.getBufferSize(), (sizeof(uint8_t) + 2 * sizeof(char)) * 8);
    packet.pushToBuffer(to_bytes('a'));
    EXPECT_EQ(packet.getBufferSize(), (sizeof(uint8_t) + 3 * sizeof(char)) * 8);
}

TEST(OperatorEquality, EmptyBuffer) {
    EXPECT_TRUE(Packet(PacketType::OPEN) == Packet(PacketType::OPEN));
    EXPECT_FALSE(Packet(PacketType::OPEN) == Packet(PacketType::CLOSE));
}

TEST(OperatorEquality, notEmptyBuffer) {
    Packet packet1(PacketType::OPEN);
    packet1.pushToBuffer(to_bytes(int(1)));

    Packet packet2(PacketType::OPEN);
    packet2.pushToBuffer(to_bytes(int(2)));

    Packet packet3(PacketType::CLOSE);
    packet3.pushToBuffer(to_bytes(int(1)));

    EXPECT_TRUE(packet1 == packet1);
    EXPECT_FALSE(packet1 == packet2);
    EXPECT_FALSE(packet1 == packet3);
    EXPECT_FALSE(packet2 == packet3);
}


TEST(OperatorLess, EmptyBuffer) {
    EXPECT_FALSE(Packet(PacketType::OPEN) < Packet(PacketType::OPEN));
    EXPECT_TRUE(Packet(PacketType::OPEN) < Packet(PacketType::CLOSE));
}

TEST(OperatorLess, notEmptyBuffer) {
    Packet packet1(PacketType::OPEN);
    packet1.pushToBuffer(to_bytes(int(1)));

    Packet packet2(PacketType::OPEN);
    packet2.pushToBuffer(to_bytes(int(2)));

    Packet packet3(PacketType::CLOSE);
    packet3.pushToBuffer(to_bytes(int(1)));

    EXPECT_FALSE(packet1 < packet1);
    EXPECT_TRUE(packet1 < packet2);
    EXPECT_TRUE(packet1 < packet3);
    EXPECT_TRUE(packet2 < packet3);
}


TEST(popFromBuffer, emptyBuffer) {
    Packet packet1(PacketType::OPEN);
    int post;

    ASSERT_THROW(packet1.popFromBuffer(post), std::runtime_error);
}

TEST(popFromBuffer, intElement) {
    Packet packet(PacketType::OPEN);
    int pre = 1;
    int post;

    packet.pushToBuffer(pre);
    EXPECT_EQ(packet.popFromBuffer(post), pre);
    EXPECT_EQ(packet.getBufferSize(), 0);
}

TEST(popFromBuffer, longElement) {
    Packet packet(PacketType::OPEN);
    long pre = 1;
    long post;

    packet.pushToBuffer(pre);
    EXPECT_EQ(packet.popFromBuffer(post), pre);
    EXPECT_EQ(packet.getBufferSize(), 0);
}

TEST(popFromBuffer, doubleElement) {
    Packet packet(PacketType::OPEN);
    double pre = 1;
    double post;

    packet.pushToBuffer(pre);
    EXPECT_EQ(packet.popFromBuffer(post), pre);
    EXPECT_EQ(packet.getBufferSize(), 0);
}

TEST(popFromBuffer, menyIntElements) {
    Packet packet(PacketType::OPEN);
    for (int i = 0; i < 10; ++i) {
        packet.pushToBuffer(i);
    }

    int post;
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(packet.popFromBuffer(post), i);
    }

    EXPECT_EQ(packet.getBufferSize(), 0);
}
