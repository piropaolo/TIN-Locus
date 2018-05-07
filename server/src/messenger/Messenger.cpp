#include "Messenger.h"
#include <climits>
#include "packet/Converter.h"

using namespace packet;

namespace messenger {

    int Messenger::send(const packet::Packet &packet) {
        send(packet.getTypeData(), packet.getTypeSize());
        for (auto &buffer : *packet.getBuffer()) {
            send(buffer.data(), bytesToBites(buffer.size()));
        }
    }

    packet::Packet Messenger::recv() {
        uint8_t header;
        from_bytes(readNBytes(sizeof(PacketType::header)), header);

        switch (PacketType::toPacketType(header)) {
            case PacketType::CLOSE:
                return Packet(PacketType::CLOSE);
            case PacketType::ACK_OK:
                return Packet(PacketType::ACK_OK);
            case PacketType::ACK_ERR:
                return Packet(PacketType::ACK_ERR);
            case PacketType::ALIVE:
                return Packet(PacketType::ALIVE);
            case PacketType::OPEN:
                return Packet(PacketType::OPEN);

            case PacketType::OPEN_ENCR:
                return Packet(PacketType::OPEN_ENCR);
            case PacketType::PUBLIC_KEY:
                return read_PUBLIC_KEY();
            case PacketType::SYMMETRIC_KEY:
                return read_SYMMETRIC_KEY();
            case PacketType::TEST_KEY:
                return read_TEST_KEY();

            case PacketType::OPEN_PROT:
                return Packet(PacketType::OPEN_PROT);
            case PacketType::SET_NAME:
                return read_SET_NAME();
            case PacketType::ADD_FOLLOWER:
                return read_ADD_FOLLOWER();
            case PacketType::REMOVE_FOLLOWER:
                return read_REMOVE_FOLLOWER();
            case PacketType::MY_LOCATION:
                return read_MY_LOCATION();

            default:
                return Packet(PacketType::UNDEFINED);
        }
    }

    const size_t Messenger::bytesToBites(const size_t &bytes) const {
        return bytes * CHAR_BIT;
    }
    
    const std::vector<std::byte> Messenger::readNBytes(const size_t &n) {
        std::vector<byte> bytes(n);
        recv(bytes.data(), bytesToBites(bytes.size()));
        return bytes;
    }

    packet::Packet Messenger::read_PUBLIC_KEY() {
        Packet packet(PacketType::PUBLIC_KEY);
        packet.getBuffer().push(readNBytes(1));
        packet.getBuffer().push(readNBytes(packet.getBuffer().frontSize()));
        return packet;
    }

    packet::Packet Messenger::read_SYMMETRIC_KEY() {
        Packet packet(PacketType::SYMMETRIC_KEY);
        packet.getBuffer().push(readNBytes(1));
        packet.getBuffer().push(readNBytes(packet.getBuffer().frontSize()));
        return packet;
    }

    packet::Packet Messenger::read_TEST_KEY() {
        Packet packet(PacketType::TEST_KEY);
        packet.getBuffer().push(readNBytes(1));
        packet.getBuffer().push(readNBytes(packet.getBuffer().frontSize()));
        return packet;
    }

    packet::Packet Messenger::read_SET_NAME() {
        Packet packet(PacketType::SET_NAME);
        packet.getBuffer().push(readNBytes(1));
        packet.getBuffer().push(readNBytes(packet.getBuffer().frontSize()));
        return packet;
    }

    packet::Packet Messenger::read_ADD_FOLLOWER() {
        Packet packet(PacketType::ADD_FOLLOWER);
        packet.getBuffer().push(readNBytes(1));
        packet.getBuffer().push(readNBytes(packet.getBuffer().frontSize()));
        return packet;
    }

    packet::Packet Messenger::read_REMOVE_FOLLOWER() {
        Packet packet(PacketType::REMOVE_FOLLOWER);
        packet.getBuffer().push(readNBytes(1));
        packet.getBuffer().push(readNBytes(packet.getBuffer().frontSize()));
        return packet;
    }

    packet::Packet Messenger::read_MY_LOCATION() {
        Packet packet(PacketType::MY_LOCATION);
        packet.getBuffer().push(readNBytes(4));
        packet.getBuffer().push(readNBytes(4));
        packet.getBuffer().push(readNBytes(8));
        return packet;
    }
}