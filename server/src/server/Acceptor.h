#ifndef LOCUS_ACCEPTOR_H
#define LOCUS_ACCEPTOR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "EPollEvent.h"
#include "message/Message.h"
#include "message/BlockingQueue.h"

//namespace {
    class Acceptor : public EPollEvent {
    public:
        explicit Acceptor(const uint16_t &port, message::BlockingQueue<message::Message> *blockingQueue);

        ~Acceptor() override = default;

        void recvData() override;

        void sendData() override {}

    private:
        struct sockaddr_in server_addr;
        message::BlockingQueue<message::Message> *blockingQueue;

        bool setNonBlock(int &sfd);
    };
//}

#endif //LOCUS_ACCEPTOR_H
