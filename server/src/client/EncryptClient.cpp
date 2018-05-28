#include "EncryptClient.h"
#include "log/Logger.h"

using namespace log;
using namespace message;
using namespace std::chrono_literals;

EncryptClient::EncryptClient(std::unique_ptr<Client> &&client)
        : DecoratorClient(std::move(client)) {

    Logger::getInstance().logMessage("EncryptClient " + std::to_string(getConnectionFD()) + ": Created");
}

void EncryptClient::recv() {
    auto msg = getBlockingQueue().pop_for(0ms);

    switch (msg.type) {
        case Message::Empty:
            Logger::getInstance().logDebug(
                    "EncryptClient " + std::to_string(getConnectionFD()) + ": No message in queue");
            break;

        case Message::Close:
            Logger::getInstance().logMessage(
                    "EncryptClient " + std::to_string(getConnectionFD()) + ": Get Close message");
            closeConnection();
            break;

        case Message::PacketSend:
            Logger::getInstance().logMessage(
                    "EncryptClient " + std::to_string(getConnectionFD()) + ": Get PacketSend message");
            break;

        case Message::PacketReceive:
            Logger::getInstance().logMessage(
                    "EncryptClient " + std::to_string(getConnectionFD()) + ": Get PacketReceive message");
            break;

        default:
            Logger::getInstance().logDebug("EncryptClient: Get unexpected message: " + msg.toString());
    }
}

void EncryptClient::sendData(const std::vector<unsigned char> &bytes) {
    //szyfruj
    client->sendData(bytes);
}

std::vector<unsigned char> EncryptClient::recvData() {
    //deszyfruj
    return client->recvData();
}
