#include "EncryptMessenger.h"

#include "packet/Converter.h"

using namespace packet;

namespace messenger {

    EncryptMessenger::EncryptMessenger(Messenger *messenger) : messenger(messenger) {}

    void EncryptMessenger::send(const void *buffer, std::size_t n) {
        encrypt(buffer, n);
        messenger->send(buffer, n);
    }

    void EncryptMessenger::recv(void *buffer, std::size_t n) {
        messenger->recv(buffer, n);
        decrypt(buffer, n);
    }

    void EncryptMessenger::init() {}

    void EncryptMessenger::encrypt(const void *buffer, size_t n) {}

    void EncryptMessenger::decrypt(void *buffer, size_t n) {}
}
