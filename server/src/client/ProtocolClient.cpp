#include "ProtocolClient.h"
#include "log/Logger.h"

using namespace Log;
using namespace message;
using namespace packet;
using namespace crypto;
using namespace std::chrono_literals;

ProtocolClient::ProtocolClient(std::unique_ptr<Client> &&client, crypto::CryptoModule &cryptoModule)
        : DecoratorClient(std::move(client)), cryptoModule(cryptoModule) {
    Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) + ": Created");
    
    cryptoModule.use(CryptoModule::Algorithm::ServerRSA);
}

void ProtocolClient::recv() {
    auto msg = getBlockingQueue().pop_for(0ms);

    switch (msg.type) {
        case Message::Empty:
            Logger::getInstance().logDebug(
                    "ProtocolClient " + std::to_string(getConnectionFD()) + ": No message in queue");
            break;

        case Message::Close:
            Logger::getInstance().logMessage(
                    "ProtocolClient " + std::to_string(getConnectionFD()) + ": Get Close message");
            closeConnection();
            break;

        case Message::PacketSend:
            Logger::getInstance().logMessage(
                    "ProtocolClient " + std::to_string(getConnectionFD()) + ": Get PacketSend message");
            sendRemainingData();
            break;

        case Message::PacketReceive:
            Logger::getInstance().logMessage(
                    "ProtocolClient " + std::to_string(getConnectionFD()) + ": Get PacketReceive message");
            receiveData();
            break;

        default:
            Logger::getInstance().logDebug("ProtocolClient " + std::to_string(getConnectionFD()) +
                                           ": Get unexpected message: " + msg.toString());
    }
}

void ProtocolClient::sendData(const std::vector<unsigned char> &bytes) {
    client->sendData(bytes);
}

std::vector<unsigned char> ProtocolClient::recvData() {
    return client->recvData();
}

void ProtocolClient::sendRemainingData() {
    if (!remainingPackets.empty()) {
        sendPacket(remainingPackets.front());
        remainingPackets.pop();
    }
}

void ProtocolClient::receiveData() {
    auto packet = recvPacket();
    switch (stage) {

        case Stage::SetPublicKey:
            setPublicKey(packet);
            break;
        case Stage::SetSymmetricKey:
            setSymmetricKey(packet);
            break;
        case Stage::Else:
            elsePacket(packet);
    }
}

void ProtocolClient::setPublicKey(packet::Packet &packet) {
    if (packet.getType() == packet::PacketType::PUBLIC_KEY) {
        //set new key
        cryptoModule.setOuterRSAKey(packet.getBuffer().popAll());
        Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                         ": Set new public key");

        cryptoModule.use(CryptoModule::Algorithm::OuterRSA);
        
        //send symmetric key
        Packet newPacket(PacketType::SYMMETRIC_KEY);
        newPacket.getBuffer().push_back(cryptoModule.getSymmetricKey());
        sendPacket(newPacket);
        Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                         ": Send symmetric key");
        
        stage = Stage::SetSymmetricKey;
    } else {
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Get wrong packet in SetPublicKey stage: " +
                                       std::to_string(PacketType::toUInt8(packet.getType())));
    }
}

void ProtocolClient::setSymmetricKey(packet::Packet &packet) {
    if (packet.getType() == packet::PacketType::SYMMETRIC_KEY) {
        //check validation of test
        if (cryptoModule.getSymmetricKey() != packet.getBuffer().popAll()) {
            Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                           ": Get wrong returning symmetric key");
            
            Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                           ": Closing connection");
            closeConnection();
        }
        Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                         ": returning symmetric key is correct");
        
        //set AES encryption
        cryptoModule.use(CryptoModule::Algorithm::AES);
        Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                         ": Set symmetric encryption");
        
        stage = Stage::Else;
    } else {
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Get wrong packet in SetSymmetricKey stage: " +
                                       std::to_string(PacketType::toUInt8(packet.getType())));
    }
}

void ProtocolClient::elsePacket(packet::Packet &packet) {
    Logger::getInstance().logMessage("ProtocolClient: Get packet");
    switch (packet.getType()) {
        case PacketType::CLOSE:
            closeConnection();
            break;
        case PacketType::SET_NAME:
            setName(packet);
            break;
        case PacketType::ADD_FOLLOWER:
            break;
        case PacketType::REMOVE_FOLLOWER:
            break;
        case PacketType::REMOVE_FOLLOWED:
            break;
        case PacketType::MY_LOCATION:
            break;

        default:
            Logger::getInstance().logError("ProtocolClient: Get wrong packet in TestSymmetricKey stage: " +
                                           PacketType::toUInt8(packet.getType()));
    }
}

void ProtocolClient::setName(packet::Packet &packet) {
    Packet newPacket(PacketType::ACK_OK);
    sendPacket(newPacket);
}
