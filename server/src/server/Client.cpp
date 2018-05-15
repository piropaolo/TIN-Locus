#include "Client.h"
#include "log/Logger.h"

using namespace log;
using namespace message;
using namespace std::chrono_literals;

Client::Client(const sockaddr &client_addr, std::unique_ptr<ClientBuffer> clientBuffer,
               message::BlockingQueue<message::Message> *clientManagerBlockingQueue)
        : client_addr(client_addr), clientBuffer(std::move(clientBuffer)), blockingQueue(1000),
          clientManagerBlockingQueue(clientManagerBlockingQueue) {
    setFileDescriptor(blockingQueue.getReadFd());

//  set clientBuffer blockingQueue to return
    this->clientBuffer->setClientBlockingQueue(&getBlockingQueue());
}

Client::~Client() {
    setFileDescriptor(0);
}

void Client::recvData() {
    auto msg = blockingQueue.pop_for(0ms);

    switch (msg.type) {
        case Message::Empty:
            Logger::getInstance().logDebug(
                    "Client " + std::to_string(clientBuffer->getFileDescriptor()) + ": No message in queue");
            break;

        case Message::Close:
            Logger::getInstance().logMessage(
                    "Client " + std::to_string(clientBuffer->getFileDescriptor()) + ": Get Close message");
            closeConnection();
            break;

        case Message::PacketSend:
            Logger::getInstance().logMessage(
                    "Client " + std::to_string(clientBuffer->getFileDescriptor()) + ": Get PacketSend message");
            break;

        case Message::PacketReceive:
            Logger::getInstance().logMessage(
                    "Client " + std::to_string(clientBuffer->getFileDescriptor()) + ": Get PacketReceive message");
            break;

        default:
            Logger::getInstance().logDebug("Client: Get unexpected message: " + msg.toString());
    }
}

BlockingQueue<Message> &Client::getBlockingQueue() {
    return blockingQueue;
}

void Client::closeConnection() {
    Message msg(Message::EraseClient);
    msg.fileDescriptor = std::make_unique<int>(clientBuffer->getFileDescriptor());
    clientManagerBlockingQueue->priorityPush(std::move(msg));
}

const sockaddr &Client::getClient_addr() const {
    return client_addr;
}
