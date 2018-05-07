#include "SimpleMessenger.h"

#include "packet/Converter.h"

using namespace packet;

namespace messenger {

    void SimpleMessenger::send(const void *buffer, std::size_t n) {}

    void SimpleMessenger::recv(void *buffer, std::size_t n) {}

    void SimpleMessenger::init() {
        Messenger::send(Packet(PacketType::OPEN));
    }
}