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

TEST(getSize, Open) {
    Packet packet(PacketType::OPEN);
    EXPECT_EQ(packet.getSize(), 1 * 8);
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

    packet.pushToBuffer(to_bytes('a'));
    packet.pushToBuffer(to_bytes('l'));
    packet.pushToBuffer(to_bytes('a'));
    EXPECT_EQ(packet.getSize(), (1 + sizeof(uint8_t) + 3 * sizeof(char)) * 8);
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
