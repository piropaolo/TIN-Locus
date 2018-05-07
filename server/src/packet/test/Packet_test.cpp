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
    EXPECT_EQ(packet.getTypeSize(), 1);
}

TEST(getBuffer, returnBuffer) {
    Packet packet(PacketType::OPEN);
    EXPECT_EQ(packet.getBuffer(), BlockBuffer());
}
