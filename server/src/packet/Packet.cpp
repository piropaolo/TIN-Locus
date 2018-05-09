#include "Packet.h"

namespace packet {

    Packet::Packet(PacketType::header type) : type(type) {}

    PacketType::header Packet::getType() const {
        return type;
    }

    const size_t Packet::getTypeSize() const {
        return sizeof type;
    }

    const std::byte *Packet::getTypeData() const {
        return reinterpret_cast< const std::byte * >( std::addressof(type));
    }

    BlockBuffer &Packet::getBuffer() {
        return buffer;
    }

    const BlockBuffer &Packet::getBuffer() const {
        return buffer;
    }

    bool Packet::operator==(const Packet &rhs) const {
        return type == rhs.type &&
               buffer == rhs.buffer;
    }

    bool Packet::operator!=(const Packet &rhs) const {
        return !(rhs == *this);
    }

    bool Packet::operator<(const Packet &rhs) const {
        if (type < rhs.type)
            return true;
        if (rhs.type < type)
            return false;
        return buffer < rhs.buffer;
    }

    bool Packet::operator>(const Packet &rhs) const {
        return rhs < *this;
    }

    bool Packet::operator<=(const Packet &rhs) const {
        return !(rhs < *this);
    }

    bool Packet::operator>=(const Packet &rhs) const {
        return !(*this < rhs);
    }
}