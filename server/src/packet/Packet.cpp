#include "Packet.h"

#include <climits>

namespace packet {

    Packet::Packet(PacketType::header type) : type(type) {}

    PacketType::header Packet::getType() const {
        return type;
    }

    const size_t Packet::getSize() const {
        return ( sizeof type + buffer.size() ) * CHAR_BIT;
    }

    void Packet::setBuffer(const std::vector<std::byte> &bytes) {
        buffer = bytes;
    }

    void Packet::pushToBuffer(const std::vector<std::byte> &bytes) {
        buffer.insert(std::end(buffer), std::begin(bytes), std::end(bytes));
    }

    const std::vector<std::byte> &Packet::getBuffer() const {
        return buffer;
    }

    const std::byte *Packet::getData() const {
        return buffer.data();
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