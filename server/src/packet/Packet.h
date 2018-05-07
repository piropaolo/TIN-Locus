#ifndef LOCUS_PACKET_H
#define LOCUS_PACKET_H

#include "PacketType.h"
#include "BlockBuffer.h"

namespace packet {
    class Packet {
    public:
        explicit Packet(PacketType::header type);

        PacketType::header getType() const;
        const size_t getTypeSize() const;
        const std::byte *getTypeData() const;

        BlockBuffer &getBuffer();
        const BlockBuffer &getBuffer() const;

        bool operator==(const Packet &rhs) const;
        bool operator!=(const Packet &rhs) const;
        
        bool operator<(const Packet &rhs) const;
        bool operator>(const Packet &rhs) const;
        bool operator<=(const Packet &rhs) const;
        bool operator>=(const Packet &rhs) const;

        int x;

    private:
        PacketType::header type = PacketType::UNDEFINED;
        BlockBuffer buffer;
    };
}


#endif //LOCUS_PACKET_H
