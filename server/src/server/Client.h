#ifndef LOCUS_CLIENT_H
#define LOCUS_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include "EPollEvent.h"
#include "message/Message.h"
#include "message/BlockingQueue.h"
#include "ClientBuffer.h"

class Client : public EPollEvent {
public:
    Client(const sockaddr &client_addr, std::unique_ptr<ClientBuffer> clientBuffer,
           message::BlockingQueue<message::Message> *clientManagerBlockingQueue);

    Client(Client&&) noexcept = default;

    ~Client() override;

    void recvData() override;

    void sendData() override {}

    const sockaddr &getClient_addr() const;

    message::BlockingQueue<message::Message> &getBlockingQueue();

private:
    struct sockaddr client_addr;
    std::unique_ptr<ClientBuffer> clientBuffer;
    message::BlockingQueue<message::Message> blockingQueue;
    message::BlockingQueue<message::Message> *clientManagerBlockingQueue;

    void closeConnection();
};


#endif //LOCUS_CLIENT_H
