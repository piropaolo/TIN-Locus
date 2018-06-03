#include <stdexcept>

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

    {
        Packet newPacket(PacketType::OPEN_ENCR);
        sendPacket(newPacket);
    }
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
    auto bytes = client->recvData();
    Logger::getInstance().logDebug("ProtocolClient: Receive bytes: " + std::to_string(bytes.size()));

    return bytes;
}

void ProtocolClient::sendRemainingData() {
    if (!remainingPackets.empty()) {
        sendPacket(remainingPackets.front());
        remainingPackets.pop();
    }
}

void ProtocolClient::receiveData() {
    switch (stage) {

        case Stage::SetPublicKey:
            setPublicKey();
            break;
        case Stage::SetSymmetricKey:
            setSymmetricKey();
            break;
        case Stage::Else:
            elsePacket();
        case Stage::TestKey:
            testKey();
        case Stage::Exit:
            break;
    }
}

void ProtocolClient::setPublicKey() {
    try {
        cryptoModule.use(CryptoModule::Algorithm::ServerRSA);
        Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                         ": Set ServerRSA encryption");
        auto packet = recvPacket();

        if (packet.getType() == packet::PacketType::PUBLIC_KEY) {
            Logger::getInstance().logMessage("ProtocolClient: Get packet: " + PacketType::toString(packet.getType()));

            //set new key
            cryptoModule.setOuterRSAKey(packet.getBuffer().popAll());
            Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                             ": Set new public key");

            cryptoModule.use(CryptoModule::Algorithm::OuterRSA);
            Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                             ": Set OuterRSA encryption");

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
                                           PacketType::toString(packet.getType()));
        }

    } catch (std::exception &e) {
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Error in SetPublicKey stage: " + e.what());
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Closing connection");
        closeConnection();
    }
}

void ProtocolClient::setSymmetricKey() {
    try {
        cryptoModule.use(CryptoModule::Algorithm::ServerRSA);
        Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                         ": Set ServerRSA encryption");
        auto packet = recvPacket();

        if (packet.getType() == packet::PacketType::SYMMETRIC_KEY) {
            Logger::getInstance().logMessage("ProtocolClient: Get packet: " + PacketType::toString(packet.getType()));

            //check validation of test
            if (cryptoModule.getSymmetricKey() != packet.getBuffer().popAll()) {
                Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                               ": Get wrong returning symmetric key");

                Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                               ": Closing connection");
                closeConnection();
            } else {
                Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                                 ": returning symmetric key is correct");

                //set AES encryption
                cryptoModule.use(CryptoModule::Algorithm::AES);
                Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                                 ": Set symmetric encryption");


                Packet newPacket(PacketType::TEST_KEY);
                newPacket.getBuffer().push_back(cryptoModule.getSymmetricKey());
                sendPacket(newPacket);
                Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                                 ": Send test key for symmetric key");

                stage = Stage::TestKey;
            }
        } else {
            Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                           ": Get wrong packet in SetSymmetricKey stage: " +
                                           PacketType::toString(packet.getType()));
        }
    } catch (std::exception &e) {
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Error in SetPublicKey stage: " + e.what());
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Closing connection");
        closeConnection();
    }
}

void ProtocolClient::testKey() {
    try {
        cryptoModule.use(CryptoModule::Algorithm::AES);
        Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                         ": Set AES encryption");
        auto packet = recvPacket();

        if (packet.getType() == packet::PacketType::TEST_KEY) {
            Logger::getInstance().logMessage("ProtocolClient: Get packet: " + PacketType::toString(packet.getType()));

            Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                             ": Test key for symmetric key was correct");

            Packet newPacket(PacketType::CLOSE);
            sendPacket(newPacket);
            Logger::getInstance().logMessage("ProtocolClient " + std::to_string(getConnectionFD()) +
                                             ": Send close");

            stage = Stage::Else;
        } else {
            Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                           ": Get wrong packet in TestKey stage: " +
                                           PacketType::toString(packet.getType()));
        }
    } catch (std::exception &e) {
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Error in SetPublicKey stage: " + e.what());
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Closing connection");
        closeConnection();
    }
}

void ProtocolClient::elsePacket() {
    try {
        auto packet = recvPacket();
        Logger::getInstance().logMessage("ProtocolClient: Get packet: " + PacketType::toString(packet.getType()));

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

            default: {
                Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                               ": Get wrong packet in Else stage: " +
                                               PacketType::toString(packet.getType()));
            }
        }
    } catch (std::exception &e) {
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Error in Else stage: " + e.what());
        Logger::getInstance().logError("ProtocolClient " + std::to_string(getConnectionFD()) +
                                       ": Closing connection");
        closeConnection();
    }
}

void ProtocolClient::setName(packet::Packet &packet) {
    Packet newPacket(PacketType::ACK_OK);
    sendPacket(newPacket);
}
