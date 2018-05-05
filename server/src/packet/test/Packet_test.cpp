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
