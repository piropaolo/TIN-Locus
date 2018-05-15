#ifndef LOCUS_BASICTHREAD_H
#define LOCUS_BASICTHREAD_H

#include "EPollManager.h"
#include "message/Message.h"
#include "message/BlockingQueue.h"

class BasicThread {
public:
    explicit BasicThread(const std::string& name);

    virtual ~BasicThread() = default;

    message::BlockingQueue<message::Message> &getBlockingQueue();

    void operator()();

protected:
    bool exit = false;
    std::string name = "";
    message::BlockingQueue<message::Message> blockingQueue;
    EPollManager ePollManager;

    void readMessage();
};

#endif //LOCUS_BASICTHREAD_H
