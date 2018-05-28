#include "SimpleClient.h"
#include "log/Logger.h"

using namespace log;
using namespace message;
using namespace std::chrono_literals;

SimpleClient::SimpleClient(const sockaddr &client_addr, std::unique_ptr<ClientBuffer> clientBuffer,
                         message::BlockingQueue<message::Message> *clientManagerBlockingQueue)
        : client_addr(client_addr), clientBuffer(std::move(clientBuffer)), blockingQueue(1000),
          clientManagerBlockingQueue(clientManagerBlockingQueue) {

    setFileDescriptor(blockingQueue.getReadFd());

//  set clientBuffer blockingQueue for return
    this->clientBuffer->setClientBlockingQueue(&getBlockingQueue());

    Logger::getInstance().logMessage("SimpleClient " + std::to_string(getConnectionFD()) + ": Created");

    upgrade();
}

SimpleClient::~SimpleClient() {
    setFileDescriptor(0);
}

void SimpleClient::recv() {
    auto msg = blockingQueue.pop_for(0ms);

    switch (msg.type) {
        case Message::Empty:
            Logger::getInstance().logDebug(
                    "SimpleClient " + std::to_string(getConnectionFD()) + ": No message in queue");
            break;

        case Message::Close:
            Logger::getInstance().logMessage(
                    "SimpleClient " + std::to_string(getConnectionFD()) + ": Get Close message");
            closeConnection();
            break;

        case Message::PacketSend:
            Logger::getInstance().logMessage(
                    "SimpleClient " + std::to_string(getConnectionFD()) + ": Get PacketSend message");
            break;

        case Message::PacketReceive:
            Logger::getInstance().logMessage(
                    "SimpleClient " + std::to_string(getConnectionFD()) + ": Get PacketReceive message");
            break;

        default:
            Logger::getInstance().logDebug("SimpleClient: Get unexpected message: " + msg.toString());
    }
}

void SimpleClient::sendData(const std::vector<unsigned char> &bytes) {
    clientBuffer->getBufferOut().push_back(const_cast<std::vector<unsigned char> &>(bytes));
    clientBuffer->getBufferOut().setStage(BlockingBuffer::Stage::Full);
    //notify sender
    clientBuffer->send();
}

std::vector<unsigned char> SimpleClient::recvData() {
    clientBuffer->getBufferOut().setStage(BlockingBuffer::Stage::Empty);
    return clientBuffer->getBufferOut().pop(clientBuffer->getBufferOut().size());
}

const sockaddr &SimpleClient::getClient_addr() const {
    return client_addr;
}

const int SimpleClient::getConnectionFD() const {
    return clientBuffer->getFileDescriptor();
}

message::BlockingQueue<message::Message> &SimpleClient::getBlockingQueue() {
    return blockingQueue;
}

void SimpleClient::closeConnection() {
    Message msg(Message::EraseClient);
    msg.fileDescriptor = std::make_unique<int>(clientBuffer->getFileDescriptor());
    clientManagerBlockingQueue->priorityPush(std::move(msg));
}

message::BlockingQueue<message::Message> &SimpleClient::getClientManagerBlockingQueue() {
    return *clientManagerBlockingQueue;
}

void SimpleClient::upgrade() {
    Message msg(Message::UpgradeClientWithEncryption);
    msg.fileDescriptor = std::make_unique<int>(getConnectionFD());
    getClientManagerBlockingQueue().push(std::move(msg));
}
