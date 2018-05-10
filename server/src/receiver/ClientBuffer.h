#ifndef LOCUS_CLIENTBUFFER_H
#define LOCUS_CLIENTBUFFER_H

//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "EPollEvent.h"
#include "Buffer.h"

class ClientBuffer : public EPollEvent {
public:
    ClientBuffer(const int &port, const sockaddr &client_addr);
    ClientBuffer(const ClientBuffer&) = delete;

    ~ClientBuffer() override = default;

    const bool isClose() const;

    const sockaddr &getClient_addr() const;

    Buffer &getBufferIn();

    Buffer &getBufferOut();

    void recvData() override;

    void sendData() override;

private:
    bool close = false;
    struct sockaddr client_addr;
    Buffer bufferIn;
    Buffer bufferOut;
};


#endif //LOCUS_CLIENTBUFFER_H
