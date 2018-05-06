#ifndef LOCUS_PROROCOLMESSENGER_H
#define LOCUS_PROROCOLMESSENGER_H

#include "Messenger.h"

namespace messenger {
    class ProrocolMessenger : public Messenger {
    public:
        explicit ProrocolMessenger(Messenger *messenger);

        int send(const void *buffer, std::size_t n) override;
        int recv(void *buffer, std::size_t n) override;

        int init() override;

    private:
        Messenger* messenger;
    };
}


#endif //LOCUS_PROROCOLMESSENGER_H
