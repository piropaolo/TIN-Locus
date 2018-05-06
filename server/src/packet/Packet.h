#ifndef LOCUS_PACKET_H
#define LOCUS_PACKET_H

#include <cstddef>
#include <vector>
#include <stdexcept>
#include "PacketType.h"
#include "Converter.h"

namespace packet {
    class Packet {
    public:
        explicit Packet(PacketType::header type);
        Packet(const Packet &) = default;
        Packet(Packet &&) = default;
        Packet &operator=(const Packet &) = default;
        Packet &operator=(Packet &&) = default;

        PacketType::header getType() const;
        const size_t getTypeSize() const;
        const std::byte *getTypeData() const;

        const size_t getBufferSize() const;
        const std::byte *getBufferData() const;
        void setBuffer(const std::vector<std::byte> &bytes);
        const std::vector<std::byte> &getBuffer() const;
        void pushToBuffer(const std::vector<std::byte> &bytes);

        template<typename T>
        void pushToBuffer(const T &object);

        template<typename T>
        T &popFromBuffer(T &object);

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


    template<typename T>
    void Packet::pushToBuffer(const T &object) {
        pushToBuffer(to_bytes(object));
    }

    template<typename T>
    T &Packet::popFromBuffer(T &object) {
        if(sizeof(T) > getBufferSize()) {
            throw std::runtime_error("Can not convert, has too small number of bytes in the buffer");
        }

        std::vector<std::byte> bytes(buffer.begin(), buffer.begin() + sizeof(T));
        buffer.erase(buffer.begin(), buffer.begin() + sizeof(T));
        from_bytes(bytes,object);

        return object;
    }
}


#endif //LOCUS_PACKET_H
