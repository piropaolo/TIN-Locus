#ifndef LOCUS_PROTOCOLCLIENT_H
#define LOCUS_PROTOCOLCLIENT_H

#include <memory>
#include <queue>
#include "DecoratorClient.h"
#include "crypto/CryptoModule.h"

class ProtocolClient : public DecoratorClient {
    enum class Stage : int {
        SetPublicKey,
        SetSymmetricKey,
        Else,
        Exit
    };
public:
    explicit ProtocolClient(std::unique_ptr<Client> &&client, crypto::CryptoModule &cryptoModule);

    ProtocolClient(ProtocolClient&&) noexcept = default;

    ~ProtocolClient() override = default;

    void recv() override;
    void send() override {}

    void sendData(const std::vector<unsigned char> &bytes) override;
    std::vector<unsigned char> recvData() override;

private:
    crypto::CryptoModule& cryptoModule;
    Stage stage = Stage::SetPublicKey;
    std::queue<packet::Packet> remainingPackets;

    void sendRemainingData();
    void receiveData();

    void setPublicKey();
    void setSymmetricKey();
    void elsePacket();

    void setName(packet::Packet &packet);
};


#endif //LOCUS_PROTOCOLCLIENT_H
