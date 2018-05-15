#ifndef LOCUS_CLIENTBUFFER_H
#define LOCUS_CLIENTBUFFER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "EPollEvent.h"
#include "Buffer.h"
#include "message/Message.h"
#include "message/BlockingQueue.h"

class ClientBuffer : public EPollEvent {
public:
    explicit ClientBuffer(const int &port);
    ClientBuffer(const ClientBuffer&) = delete;

    ~ClientBuffer() override = default;

    const bool isClose() const;

    Buffer &getBufferIn();

    Buffer &getBufferOut();

    void setClientBlockingQueue(message::BlockingQueue<message::Message> *clientBlockingQueue);

    void recvData() override;

    void sendData() override;

private:
    mutable bool close = false;
    Buffer bufferIn;
    Buffer bufferOut;
    message::BlockingQueue<message::Message> *clientBlockingQueue = nullptr;

    std::vector<std::byte> recvData(const size_t &n) const;
    std::vector<std::byte>  sendData(std::vector<std::byte> buffer) const;
};


#endif //LOCUS_CLIENTBUFFER_H
