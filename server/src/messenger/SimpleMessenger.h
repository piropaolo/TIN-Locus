#ifndef LOCUS_SIMPLEMESSENGER_H
#define LOCUS_SIMPLEMESSENGER_H

#include "Messenger.h"

namespace messenger {
    class SimpleMessenger : public Messenger {
    public:
        int send(const void *buffer, std::size_t n) override;
        int recv(void *buffer, std::size_t n) override;

        int init() override;
    };
}


#endif //LOCUS_SIMPLEMESSENGER_H
