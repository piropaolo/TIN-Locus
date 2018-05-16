#include "ProtocolClient.h"
#include "log/Logger.h"

using namespace log;
using namespace message;
using namespace std::chrono_literals;

ProtocolClient::ProtocolClient(std::unique_ptr<Client> &&client)
        : DecoratorClient(std::move(client)) {
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
            break;

        case Message::PacketReceive:
            Logger::getInstance().logMessage(
                    "ProtocolClient " + std::to_string(getConnectionFD()) + ": Get PacketReceive message");
            break;

        default:
            Logger::getInstance().logDebug("ProtocolClient: Get unexpected message: " + msg.toString());
    }
}

void ProtocolClient::sendData(const std::vector<std::byte> &bytes) {
    client->sendData(bytes);
}

std::vector<std::byte> ProtocolClient::recvData() {
    return client->recvData();
}

void ProtocolClient::upgrade() {
    Message msg(Message::UpgradeClientWithProtocol);
    msg.fileDescriptor = std::make_unique<int>(getConnectionFD());
    getClientManagerBlockingQueue().push(std::move(msg));
}
