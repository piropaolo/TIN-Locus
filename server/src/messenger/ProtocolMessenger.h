#ifndef LOCUS_PROTOCOLMESSENGER_H
#define LOCUS_PROTOCOLMESSENGER_H

#include "Messenger.h"

namespace messenger {
    class ProtocolMessenger : public Messenger {
    public:
        explicit ProtocolMessenger(Messenger *messenger);

        void send(const void *buffer, std::size_t n) override;
        void recv(void *buffer, std::size_t n) override;

        void init() override;

    private:
        Messenger* messenger;
    };
}


#endif //LOCUS_PROTOCOLMESSENGER_H
