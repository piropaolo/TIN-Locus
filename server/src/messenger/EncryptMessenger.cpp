#include "EncryptMessenger.h"

#include "packet/Converter.h"

using namespace packet;

namespace messenger {

    EncryptMessenger::EncryptMessenger(Messenger *messenger) : messenger(messenger) {}

    int EncryptMessenger::send(const void *buffer, std::size_t n) {
        encrypt(buffer, n);
        return messenger->send(buffer, n);
    }

    int EncryptMessenger::recv(void *buffer, std::size_t n) {
        messenger->recv(buffer, n);
        decrypt(buffer, n);
        return 0;
    }

    int EncryptMessenger::init() {
        return 0;
    }

    void EncryptMessenger::encrypt(const void *buffer, size_t n) {}

    void EncryptMessenger::decrypt(void *buffer, size_t n) {}
}
