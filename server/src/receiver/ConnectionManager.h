#ifndef LOCUS_CONNECTIONMANAGER_H
#define LOCUS_CONNECTIONMANAGER_H

#include <sys/epoll.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "message/Message.h"
#include "message/BlockingQueue.h"

#include "EPollEvent.h"

//namespace { ConnectionManager
class ConnectionManager {
    struct epoll_event event;

public:
    explicit ConnectionManager(const std::string& name);

    virtual ~ConnectionManager();

    message::BlockingQueue<message::Message> &getBlockingQueue();

    void addSocket(EPollEvent *newEvent);

    void eraseSocket(const int &socketID);

    uint32_t getFlags() const;

    void setFlags(const uint32_t &flags);

    void operator()();

private:
    bool exit = false;
    std::string name = "";
    message::BlockingQueue<message::Message> blockingQueue;
    int epollFD = 0;
    uint32_t flags = 0;
    const int maxSizeEvents = 1024;
    std::vector<epoll_event> events;
//    std::unordered_map<int, EPollEvent *> EPollEventMap;

    void readMessage();

    void readEPoll();
};
//}


#endif //LOCUS_CONNECTIONMANAGER_H
