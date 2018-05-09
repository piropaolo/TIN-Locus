#include "ProtocolMessenger.h"

#include "packet/Converter.h"

using namespace packet;

namespace messenger {

    ProtocolMessenger::ProtocolMessenger(Messenger *messenger) : messenger(messenger) {}

    void ProtocolMessenger::send(const void *buffer, std::size_t n) {
        return messenger->send(buffer, n);
    }

    void ProtocolMessenger::recv(void *buffer, std::size_t n) {
        messenger->recv(buffer, n);
    }

    void ProtocolMessenger::init() {}
}