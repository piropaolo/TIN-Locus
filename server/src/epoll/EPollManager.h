#ifndef LOCUS_EPOLLMANAGER_H
#define LOCUS_EPOLLMANAGER_H

#include <sys/epoll.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "message/Message.h"
#include "message/BlockingQueue.h"

#include "EPollEvent.h"

//namespace { EPollManager
class EPollManager {
    struct epoll_event event;

public:
    explicit EPollManager(const std::string& name);

    virtual ~EPollManager();

    void addEPollEvent(EPollEvent *newEvent);

    void changeEPollEvent(EPollEvent *newEvent);

    void eraseFileDescriptor(const int &socketID);

    uint32_t getFlags() const;

    void setFlags(const uint32_t &flags);

    const std::string &getName() const;

    void readEPoll();

private:
    std::string name = "";
    int ePollFD = 0;
    uint32_t flags = 0;
    const int maxSizeEvents = 1024;
    std::vector<epoll_event> events;
};
//}


#endif //LOCUS_EPOLLMANAGER_H
