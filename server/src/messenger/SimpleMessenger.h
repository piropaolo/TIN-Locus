#ifndef LOCUS_SIMPLEMESSENGER_H
#define LOCUS_SIMPLEMESSENGER_H

#include "Messenger.h"

namespace messenger {
    class SimpleMessenger : public Messenger {
    public:
        void send(const void *buffer, std::size_t n) override;
        void recv(void *buffer, std::size_t n) override;
        std::size_t receiveAny(void *buffer, std::size_t max_bytes) override;

        void init() override;
        void close() override;

        SimpleMessenger(int socket_fd);
        ~SimpleMessenger();

    private:
        int socketFd_;
        bool socketOpen_;
    };
}


#endif //LOCUS_SIMPLEMESSENGER_H
