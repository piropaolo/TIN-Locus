#ifndef LOCUS_DECORATORCLIENT_H
#define LOCUS_DECORATORCLIENT_H

#include <memory>
#include "Client.h"

class DecoratorClient : public Client {
public:
    explicit DecoratorClient(std::unique_ptr<Client> &&client);

    DecoratorClient(DecoratorClient&&) noexcept = default;

    ~DecoratorClient() override = default;

    const sockaddr &getClient_addr() const override;

    int getFileDescriptor() const override;

    const int getConnectionFD() const override;

    message::BlockingQueue<message::Message> &getBlockingQueue() override;

    message::BlockingQueue<message::Message> &getClientManagerBlockingQueue() override;

    void closeConnection() override;

protected:
    std::unique_ptr<Client> client;
};


#endif //LOCUS_DECORATORCLIENT_H
