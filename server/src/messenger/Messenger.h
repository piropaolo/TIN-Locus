#ifndef LOCUS_MESSENGER_H
#define LOCUS_MESSENGER_H

#include <cstddef>
#include "packet/Packet.h"

namespace messenger {
    class Messenger {
    public:
        virtual int send(const void *buffer, std::size_t n) = 0;
        virtual int recv(void *buffer, std::size_t n) = 0;

        virtual int send(const packet::Packet &packet);
        virtual packet::Packet recv();

        virtual int init() = 0;

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
