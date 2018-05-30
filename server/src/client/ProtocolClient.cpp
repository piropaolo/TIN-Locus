#include "ProtocolClient.h"
#include "log/Logger.h"
#include "buffer/RandomStringGenerator.h"

using namespace Log;
using namespace message;
using namespace packet;
using namespace crypto;
using namespace std::chrono_literals;

ProtocolClient::ProtocolClient(std::unique_ptr<Client> &&client, crypto::CryptoModule &cryptoModule)
        : DecoratorClient(std::move(client)), cryptoModule(cryptoModule) {
    Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) + ": Created");
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
            Logger::getInstance().logDebug("ProtocolClient: Get unexpected message: " + msg.toString());
    }
}

void ProtocolClient::sendData(const std::vector<unsigned char> &bytes) {
    client->sendData(bytes);
}

std::vector<unsigned char> ProtocolClient::recvData() {
    return client->recvData();
}

void ProtocolClient::sendRemainingData() {
    if(!remainingPackets.empty()) {
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
        case Stage::TestPublicKey:
            testPublicKey(packet);
            break;
        case Stage::SetSymmetricKey:
            setSymmetricKey(packet);
            break;
        case Stage::TestSymmetricKey:
            testSymmetricKey(packet);
            break;
        case Stage::Else:
            elsePacket(packet);
    }
}

void ProtocolClient::setPublicKey(packet::Packet &packet) {
    switch (packet.getType()) {
        case packet::PacketType::PUBLIC_KEY: {
            //set new key
            cryptoModule.setOuterRSAKey(packet.getBuffer().popAll());
            Logger::getInstance().logMessage("ProtocolClient: Set new public key");

            //send test for public key
            Packet newPacket(PacketType::TEST_KEY);
            testKey = randomStringGenerator(30);
            newPacket.getBuffer().push_back(testKey);
            sendPacket(newPacket);
            Logger::getInstance().logMessage("ProtocolClient: Send testPublicKey");

            stage = Stage::TestPublicKey;
            break;
        }
        default:
            Logger::getInstance().logError("ProtocolClient: Get wrong packet in SetPublicKey stage: " +
                                           PacketType::toUInt8(packet.getType()));
    }
}

void ProtocolClient::testPublicKey(packet::Packet &packet) {
    switch (packet.getType()) {
        case packet::PacketType::PUBLIC_KEY: {
            //check validation of test
            if (testKey != packet.getBuffer().popAll()) {
                Logger::getInstance().logError("ProtocolClient: Get wrong testPublicKey: " +
                                               PacketType::toUInt8(packet.getType()));
                Logger::getInstance().logError("ProtocolClient: Closing connection");
                closeConnection();
            }
            Logger::getInstance().logMessage("ProtocolClient: testPublicKey is correct");

            //send symmetric key
            Packet newPacket(PacketType::SYMMETRIC_KEY);
            newPacket.getBuffer().push_back(cryptoModule.getSymmetricKey());
            sendPacket(newPacket);
            Logger::getInstance().logMessage("ProtocolClient: Send symmetric key");

            stage = Stage::SetSymmetricKey;
            break;
        }
        default:
            Logger::getInstance().logError("ProtocolClient: Get wrong packet in SetPublicKey stage: " +
                                           PacketType::toUInt8(packet.getType()));
    }
}

void ProtocolClient::setSymmetricKey(packet::Packet &packet) {
    switch (packet.getType()) {
        case packet::PacketType::ACK_OK: {
            //set AES encryption
            cryptoModule.use(CryptoModule::Algorithm::AES);
            Logger::getInstance().logMessage("ProtocolClient: Set symmetric encryption");

            //send test for key
            Packet newPacket(PacketType::TEST_KEY);
            testKey = randomStringGenerator(30);
            newPacket.getBuffer().push_back(testKey);
            sendPacket(newPacket);
            Logger::getInstance().logMessage("ProtocolClient: Send TestSymmetricKey");

            stage = Stage::TestSymmetricKey;
            break;
        }
        default:
            Logger::getInstance().logError("ProtocolClient: Get wrong packet in SetSymmetricKey stage: " +
                                           PacketType::toUInt8(packet.getType()));
    }
}

void ProtocolClient::testSymmetricKey(packet::Packet &packet) {
    switch (packet.getType()) {
        case packet::PacketType::TEST_KEY: {
            //check validation of test
            if (testKey != packet.getBuffer().popAll()) {
                Logger::getInstance().logError("ProtocolClient: Get wrong test: " +
                                               PacketType::toUInt8(packet.getType()));
                Logger::getInstance().logError("ProtocolClient: Closing connection");
                closeConnection();
            }
            Logger::getInstance().logMessage("ProtocolClient: TestSymmetricKey is correct");

            Logger::getInstance().logMessage("ProtocolClient: Connection is stabilized");
            stage = Stage::Else;
            break;
        }
        default:
            Logger::getInstance().logError("ProtocolClient: Get wrong packet in TestSymmetricKey stage: " +
                                           PacketType::toUInt8(packet.getType()));
    }
}

void ProtocolClient::elsePacket(packet::Packet &packet) {
    Logger::getInstance().logMessage("ProtocolClient: Get packet");
}
