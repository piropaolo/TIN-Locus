#ifndef LOCUS_PACKET_H
#define LOCUS_PACKET_H

#include <cstddef>
#include <vector>
#include "PacketType.h"

namespace packet {
    class Packet {
    public:
        explicit Packet(PacketType::header type);
        Packet(const Packet &) = default;
        Packet(Packet &&) = default;
        Packet &operator=(const Packet &) = default;
        Packet &operator=(Packet &&) = default;

        PacketType::header getType() const;

        const size_t getSize() const;

        void setBuffer(const std::vector<std::byte> &bytes);

        void pushToBuffer(const std::vector<std::byte> &bytes);

        const std::vector<std::byte> &getBuffer() const;

        const std::byte *getData() const;

        bool operator==(const Packet &rhs) const;
        bool operator!=(const Packet &rhs) const;

        bool operator<(const Packet &rhs) const;
        bool operator>(const Packet &rhs) const;
        bool operator<=(const Packet &rhs) const;
        bool operator>=(const Packet &rhs) const;

    private:
        PacketType::header type = PacketType::UNDEFINED;
        std::vector<std::byte> buffer;
    };
}


#endif //LOCUS_PACKET_H
