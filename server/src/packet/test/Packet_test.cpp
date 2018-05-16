#include "gtest/gtest.h"
#include "../Packet.h"

#include "buffer/Converter.h"

using namespace packet;

TEST(Packet_getType, Open) {
    Packet packet(PacketType::OPEN);
    EXPECT_EQ(packet.getType(), PacketType::OPEN);
}

TEST(Packet_getType, Close) {
    Packet packet(PacketType::CLOSE);
    EXPECT_EQ(packet.getType(), PacketType::CLOSE);
}

TEST(Packet_getType, Alive) {
    Packet packet(PacketType::ALIVE);
    EXPECT_EQ(packet.getType(), PacketType::ALIVE);
}

TEST(Packet_getTypeSize, Open) {
    Packet packet(PacketType::OPEN);
    EXPECT_EQ(packet.getTypeSize(), 1);
}

TEST(Packet_getBuffer, returnBuffer) {
    Packet packet(PacketType::OPEN);
    EXPECT_EQ(packet.getBuffer(), BlockBuffer());
}
