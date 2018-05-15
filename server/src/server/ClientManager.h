#ifndef LOCUS_CLIENTMANAGER_H
#define LOCUS_CLIENTMANAGER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include "EPollEvent.h"
#include "message/Message.h"
#include "message/BlockingQueue.h"
#include "Server.h"
#include "Receiver.h"
#include "Sender.h"
#include "ClientBuffer.h"
#include "Client.h"

class ClientManager : public EPollEvent {
public:
    explicit ClientManager(Server &server, Receiver &receiver, Sender &sender);

    ~ClientManager() override;

    message::BlockingQueue<message::Message> &getBlockingQueue();

    void recvData() override;

    void sendData() override {}

private:
    message::BlockingQueue<message::Message> blockingQueue;
    Server &server;
    Receiver &receiver;
    Sender &sender;
    std::unordered_map<int, std::unique_ptr<Client>> clientsMap;
//    std::unordered_map<int, Client> clientsMap;

    void addClient(const int &fileDescriptor, const sockaddr &sock_addr);

    void eraseClient(const int &fileDescriptor);

    void closeAllClients();

};


#endif //LOCUS_CLIENTMANAGER_H
