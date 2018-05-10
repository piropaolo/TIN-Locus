#include <iostream>
#include <thread>
#include "receiver/Acceptor.h"
#include "receiver/ConnectionManager.h"
#include "message/Message.h"
#include "message/BlockingQueue.h"
#include "receiver/ClientBuffer.h"

using namespace message;

const uint16_t PORT = 4444;

int main() {
    ConnectionManager recv("Receiver");
    auto recQueue = &recv.getBlockingQueue();
    recv.setFlags(EPOLLIN | EPOLLET);
    std::thread recvThread([&recv] { recv(); });

    ConnectionManager send("Sender");
    auto sendQueue = &send.getBlockingQueue();
    send.setFlags(EPOLLOUT | EPOLLET);
    std::thread sendThread([&send] { send(); });

    BlockingQueue<Message> blockingQueue(100);
    Message msg(Message::AddSocket);
    Acceptor acceptor(PORT, &blockingQueue);
    msg.ePollEvent = std::make_unique<EPollEvent *>(&acceptor);
    recQueue->push(std::move(msg));

    std::list<std::unique_ptr<ClientBuffer>> clientBufferList;
    while (!std::cin.eof()) {
        msg = blockingQueue.pop();

        std::unique_ptr<ClientBuffer> clientBuffer = std::make_unique<ClientBuffer>(*msg.socketID, *msg.sock_addr);

        msg = Message(Message::AddSocket);
        msg.ePollEvent = std::make_unique<EPollEvent *>(clientBuffer.get());;
        recQueue->push(std::move(msg));

        msg = Message(Message::AddSocket);
        msg.ePollEvent = std::make_unique<EPollEvent *>(clientBuffer.get());
        sendQueue->push(std::move(msg));

        auto bytes = packet::to_bytes("hello");
        clientBuffer->getBufferOut().push_back(bytes);
        clientBuffer->sendData();

        clientBufferList.push_back(std::move(clientBuffer));
    }

    msg = Message(Message::Close);
    recQueue->push(std::move(msg));
    recvThread.join();

    msg = Message(Message::Close);
    sendQueue->push(std::move(msg));
    sendThread.join();

    return 0;
}