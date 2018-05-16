#include "Packet.h"

namespace packet {

    Packet::Packet(PacketType::header type) : type(type) {}

    PacketType::header Packet::getType() const {
        return type;
    }

    Buffer &Packet::getBuffer() {
        return buffer;
    }

    const Buffer &Packet::getBuffer() const {
        return buffer;
    }
}