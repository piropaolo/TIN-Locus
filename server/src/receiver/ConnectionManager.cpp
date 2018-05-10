#include "ConnectionManager.h"

#include "log/Logger.h"
#include <string>
#include <chrono>
#include <unistd.h>

using namespace log;
using namespace message;
using namespace std::chrono_literals;

//namespace {

ConnectionManager::ConnectionManager(const std::string &name)
        : name(name), blockingQueue(1000), events(static_cast<unsigned long>(maxSizeEvents)) {
    Logger::getInstance().logMessage(name + ": Initialize");
    epollFD = epoll_create1(0);
    if (epollFD < 0) {
        throw std::runtime_error(name + ": Error while creating epoll: " + std::to_string(epollFD));
    }
}

ConnectionManager::~ConnectionManager() {
    Logger::getInstance().logMessage(name + ": Closing");
    close(epollFD);
}

message::BlockingQueue<message::Message> &ConnectionManager::getBlockingQueue() {
    return blockingQueue;
}

void ConnectionManager::addSocket(EPollEvent *newEvent) {
    event.data.ptr = newEvent;
    event.events = flags; //EPOLLIN EPOLLOUT

    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, newEvent->getFileDescriptor(), &event) < 0) {
        throw std::runtime_error(name + ": Error while adding to epoll_ctl, inserting fd: " +
                                 std::to_string(newEvent->getFileDescriptor()));
    }

    Logger::getInstance().logMessage(
            name + ": The socket is added: " + std::to_string(newEvent->getFileDescriptor()));
}

void ConnectionManager::eraseSocket(const int &socketID) {
    epoll_ctl(epollFD, EPOLL_CTL_DEL, socketID, nullptr);

    Logger::getInstance().logMessage(name + ": The socket is erased: " + std::to_string(socketID));
}

void ConnectionManager::operator()() {
    Logger::getInstance().logMessage(name + ": Starting");
    while (!exit) {
        readMessage();
        readEPoll();
    }
}

void ConnectionManager::readMessage() {
    while (true) {
        auto msg = blockingQueue.pop_for(500ms);

        switch (msg.type) {
            case Message::Empty:
//                    Logger::getInstance().logDebug("ConnectionManager: No message in queue");
                return;
            case Message::Close:
                Logger::getInstance().logMessage(name + ": Get Close message");
                exit = true;
                return;
            case Message::AddSocket:
                if (!msg.ePollEvent) {
                    Logger::getInstance().logDebug(name + ": AddSocket message doesn't contain ePollEvent");
                } else {
                    Logger::getInstance().logMessage(name + ": Get AddSocket message");
                    addSocket(*msg.ePollEvent);
                }
                break;
            case Message::EraseSocket:
                if (!msg.socketID) {
                    Logger::getInstance().logDebug(name + ": AddSocket message doesn't contain socketID");
                } else {
                    Logger::getInstance().logMessage(name + ": Get AddSocket message");
                    eraseSocket(*msg.socketID);
                }
                break;
            default:
                Logger::getInstance().logDebug(name + ": Get unexpected message: " + msg.toString());
        }
    }
}

void ConnectionManager::readEPoll() {
    int nfds = epoll_wait(epollFD, events.data(), maxSizeEvents, 0);

    if (nfds < 0) {
        Logger::getInstance().logError(name + ": epoll_wait error: " + std::to_string(nfds));
    } else if (nfds > 0) {
        Logger::getInstance().logDebug(name + ": epoll_wait return: " + std::to_string(nfds));
    }

    for (int i = 0; i < nfds; ++i) {
        if (events[i].events & EPOLLIN) {
            static_cast<EPollEvent *>(events[i].data.ptr)->recvData();
        } else if (events[i].events & EPOLLOUT) {
            static_cast<EPollEvent *>(events[i].data.ptr)->sendData();
        } else {
            Logger::getInstance().logError(
                    name + ": Error on file descriptor " + std::to_string(events[i].events));
        }
    }
}

uint32_t ConnectionManager::getFlags() const {
    return flags;
}

void ConnectionManager::setFlags(const uint32_t &flags) {
    ConnectionManager::flags = flags;
}
//}