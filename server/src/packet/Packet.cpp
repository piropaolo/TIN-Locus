#include "Packet.h"

#include <climits>

namespace packet {

    Packet::Packet(PacketType::header type) : type(type) {}

    PacketType::header Packet::getType() const {
        return type;
    }

    const size_t Packet::getTypeSize() const {
        return sizeof type * CHAR_BIT;
    }

    const std::byte *Packet::getTypeData() const {
        return reinterpret_cast< const std::byte * >( std::addressof(type));
    }

    const size_t Packet::getBufferSize() const {
        return buffer.size() * CHAR_BIT;
    }

    const std::byte *Packet::getBufferData() const {
        return buffer.data();
    }

    void Packet::setBuffer(const std::vector<std::byte> &bytes) {
        buffer = bytes;
    }

    const std::vector<std::byte> &Packet::getBuffer() const {
        return buffer;
    }

    void Packet::pushToBuffer(const std::vector<std::byte> &bytes) {
        buffer.insert(std::end(buffer), std::begin(bytes), std::end(bytes));
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