#ifndef LOCUS_ENCRYPTMESSENGER_H
#define LOCUS_ENCRYPTMESSENGER_H

#include "Messenger.h"

namespace messenger {
    class EncryptMessenger : public Messenger {
    public:
        explicit EncryptMessenger(Messenger *messenger);

        int send(const void *buffer, std::size_t n) override;
        int recv(void *buffer, std::size_t n) override;

        int init() override;

    private:
        Messenger* messenger;

        void encrypt(const void *buffer, size_t n);

        void decrypt(void *buffer, size_t n);
    };
}


#endif //LOCUS_ENCRYPTMESSENGER_H
