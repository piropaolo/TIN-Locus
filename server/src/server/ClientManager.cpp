#include "ClientManager.h"
#include "log/Logger.h"

#include <iostream>

using namespace log;
using namespace message;
using namespace std::chrono_literals;


ClientManager::ClientManager(Server &server, Receiver &receiver, Sender &sender)
        : blockingQueue(1000), server(server), receiver(receiver), sender(sender) {
    setFileDescriptor(blockingQueue.getReadFd());
    Logger::getInstance().logMessage("ClientManager: My fd is " + std::to_string(getFileDescriptor()));
}

ClientManager::~ClientManager() {
    setFileDescriptor(0);
}

BlockingQueue<Message> &ClientManager::getBlockingQueue() {
    return blockingQueue;
}

void ClientManager::recvData() {
    auto msg = blockingQueue.pop_for(0ms);

    switch (msg.type) {
        case Message::Empty:
            Logger::getInstance().logDebug("ClientManager: No message in queue");
            break;
        case Message::Close:
            Logger::getInstance().logMessage("ClientManager: Get Close message");
            closeAllClients();
            break;
        case Message::AddClient:
            if (!msg.fileDescriptor) {
                Logger::getInstance().logDebug(
                        "ClientManager: AddClient message doesn't contain fileDescriptor");
            } else if (!msg.sock_addr) {
                Logger::getInstance().logDebug(
                        "ClientManager: AddClient message doesn't contain sock_addr");
            } else {
                Logger::getInstance().logMessage("ClientManager: Get AddClient message");
                addClient(*msg.fileDescriptor, *msg.sock_addr);
            }
            break;
        case Message::EraseClient:
            if (!msg.fileDescriptor) {
                Logger::getInstance().logDebug(
                        "ClientManager: EraseClient message doesn't contain fileDescriptor");
            } else {
                Logger::getInstance().logMessage("ClientManager: Get EraseClient message");
                eraseClient(*msg.fileDescriptor);
            }
            break;
        default:
            Logger::getInstance().logDebug("ClientManager: Get unexpected message: " + msg.toString());
    }
}

void ClientManager::addClient(const int &fileDescriptor, const sockaddr &sock_addr) {
    //Create ClientBuffer
    std::unique_ptr<ClientBuffer> clientBuffer = std::make_unique<ClientBuffer>(fileDescriptor);


    {//Add buffer to receiver
        Message msg = Message(Message::AddEPollEvent);
        msg.ePollEvent = std::make_unique<EPollEvent *>(clientBuffer.get());;
        receiver.getBlockingQueue().push(std::move(msg));
    }

    {//Add buffer to sender
        Message msg = Message(Message::AddEPollEvent);
        msg.ePollEvent = std::make_unique<EPollEvent *>(clientBuffer.get());
        sender.getBlockingQueue().push(std::move(msg));
    }

    //Create Client
    clientsMap.emplace(fileDescriptor,
                       std::make_unique<Client>(sock_addr, std::move(clientBuffer), &getBlockingQueue()));

    {//Add client to server
        Message msg = Message(Message::AddEPollEvent);
        msg.ePollEvent = std::make_unique<EPollEvent *>(&*clientsMap.at(fileDescriptor));
        server.getBlockingQueue().push(std::move(msg));
    }
}

void ClientManager::eraseClient(const int &fileDescriptor) {
    auto it = clientsMap.find(fileDescriptor);
    if (it == clientsMap.end()) {
        Logger::getInstance().logDebug("ClientManager: fileDescriptor not register");
        return;
    }

    Logger::getInstance().logMessage("ClientManager: Erase client with fd: " + std::to_string(fileDescriptor));
    clientsMap.erase(fileDescriptor);
}

void ClientManager::closeAllClients() {}
