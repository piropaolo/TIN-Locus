#ifndef LOCUS_MESSENGER_H
#define LOCUS_MESSENGER_H

#include <cstddef>
#include "../packet/Packet.h"

namespace messenger {
    class Messenger {
    public:
        virtual void send(const void *buffer, std::size_t n) = 0;
        virtual void recv(void *buffer, std::size_t n) = 0;
        virtual std::size_t receiveAny(void *buffer, std::size_t max_bytes) = 0;

        virtual void send(const packet::Packet &packet);
        virtual packet::Packet recv();

        virtual void init() = 0;
        virtual void close() = 0;

        virtual ~Messenger();

    private:
        const size_t bytesToBites(const size_t &bytes) const;
        const std::vector<std::byte> readNBytes(const size_t &n);

        packet::Packet read_SET_NAME();
        packet::Packet read_ADD_FOLLOWER();
        packet::Packet read_REMOVE_FOLLOWER();
        packet::Packet read_MY_LOCATION();
        packet::Packet read_PUBLIC_KEY();
        packet::Packet read_SYMMETRIC_KEY();
        packet::Packet read_TEST_KEY();
    };
}


#endif //LOCUS_MESSENGER_H
