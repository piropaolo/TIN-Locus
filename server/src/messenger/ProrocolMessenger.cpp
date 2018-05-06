#include "ProrocolMessenger.h"

#include "packet/Converter.h"

using namespace packet;

namespace messenger {

    ProrocolMessenger::ProrocolMessenger(Messenger *messenger) : messenger(messenger) {}

    int ProrocolMessenger::send(const void *buffer, std::size_t n) {
        return messenger->send(buffer, n);
    }

    int ProrocolMessenger::recv(void *buffer, std::size_t n) {
        messenger->recv(buffer, n);
    }

    int ProrocolMessenger::init() {
        return 0;
    }
}