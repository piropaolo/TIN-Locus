#ifndef LOCUS_PROTOCOLCLIENT_H
#define LOCUS_PROTOCOLCLIENT_H

#include <memory>
#include "DecoratorClient.h"

class ProtocolClient : public DecoratorClient {
public:
    explicit ProtocolClient(std::unique_ptr<Client> &&client);

    ProtocolClient(ProtocolClient&&) noexcept = default;

    ~ProtocolClient() override = default;

    void recv() override;
    void send() override {}

    void sendData(const std::vector<std::byte> &bytes) override;
    std::vector<std::byte> recvData() override;

private:
    void upgrade();
};


#endif //LOCUS_PROTOCOLCLIENT_H
