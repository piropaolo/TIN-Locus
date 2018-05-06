#include "SimpleMessenger.h"

#include "packet/Converter.h"

using namespace packet;

namespace messenger {

    int SimpleMessenger::send(const void *buffer, std::size_t n) {
        return 0;
    }

    int SimpleMessenger::recv(void *buffer, std::size_t n) {
        return 0;
    }

    int SimpleMessenger::init() {
        Messenger::send(Packet(PacketType::OPEN));
        return 0;
    }
}