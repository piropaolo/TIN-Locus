#ifndef LOCUS_SIMPLEMESSENGER_H
#define LOCUS_SIMPLEMESSENGER_H

#include "Messenger.h"

namespace messenger {
    class SimpleMessenger : public Messenger {
    public:
        void send(const void *buffer, std::size_t n) override;
        void recv(void *buffer, std::size_t n) override;

        void init() override;
    };
}


#endif //LOCUS_SIMPLEMESSENGER_H
