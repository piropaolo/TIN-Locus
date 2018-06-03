#include "ClientDataManager.h"

#include <stdexcept>
#include "buffer/Converter.h"

using namespace buffer;
using namespace message;

int ClientDataManager::addClient(const std::vector<unsigned char> &publicKey) {
    clientId.insert({toString(publicKey), nextId});
    clientInfo.insert({nextId, ClientInfo(publicKey)});
    return nextId++;
}

int ClientDataManager::registerClient(const std::vector<unsigned char> &publicKey,
                                          message::BlockingQueue<message::Message> *blockingQueue) {
    int id = 0;

    auto clientIdIt = clientId.find(toString(publicKey));
    if(clientIdIt != clientId.end()) {
        id = clientIdIt->second;
    } else {
        id = addClient(publicKey);
    }

    clientsQueue.insert({id, blockingQueue});
    return id;
}

void ClientDataManager::unregister(const int &id) {
    clientsQueue.erase(id);
}

void ClientDataManager::addObserver(const int &myId, const int &observerId) {
    auto myIdIt = clientInfo.find(myId);
    if(myIdIt == clientInfo.end()) {
        std::runtime_error("Error in addObserver method. myId does not exist");
    }
    auto observerIdIt = clientInfo.find(observerId);
    if( observerIdIt == clientInfo.end()) {
        std::runtime_error("Error in addObserver method. observerId does not exist");
    }

    myIdIt->second.addObserver(observerId);
    observerIdIt->second.addWatcher(myId);

    auto observerQueue = clientsQueue.find(observerId);
    if(observerQueue != clientsQueue.end()) {
        observerQueue->second->priorityPush(Message(Message::Type::Update));
    }
}

void ClientDataManager::eraseObserver(const int &myId, const int &observerId) {
    auto myIdIt = clientInfo.find(myId);
    if(myIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseObserver method. myId does not exist");
    }
    auto observerIdIt = clientInfo.find(observerId);
    if(observerIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseObserver method. observerId does not exist");
    }

    myIdIt->second.eraseObserver(observerId);
    observerIdIt->second.eraseWatcher(myId);

    auto observerQueue = clientsQueue.find(observerId);
    if(observerQueue != clientsQueue.end()) {
        Message message(Message::Type::EraseObserver);
        message.id = std::make_unique<int>(myId);
        observerQueue->second->priorityPush(std::move(message));
    }
}

void ClientDataManager::eraseWatcher(const int &myId, const int &watcherId) {
    auto myIdIt = clientInfo.find(myId);
    if(myIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseWatcher method. myId does not exist");
    }
    auto watcherIdIt = clientInfo.find(watcherId);
    if(watcherIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseWatcher method. watcherId does not exist");
    }

    myIdIt->second.eraseWatcher(watcherId);
    watcherIdIt->second.eraseObserver(myId);
}

void ClientDataManager::notifyObservers(const int &id) {
    auto clientInfoIt = clientInfo.find(id);
    if(clientInfoIt == clientInfo.end()) {
        std::runtime_error("Error in notifyObservers method. id does not exist");
    }

    for (auto &observer : clientInfoIt->second.observers) {
        auto observerQueue = clientsQueue.find(observer);
        if(observerQueue != clientsQueue.end()) {
            observerQueue->second->priorityPush(Message(Message::Type::Update));
        }
    }
}

void
ClientDataManager::addPosition(const int &myId, const float &latitude, const float &longitude, const time_t &time) {
    auto myIdIt = clientInfo.find(myId);
    if(myIdIt == clientInfo.end()) {
        std::runtime_error("Error in addPosition method. myId does not exist");
    }

    myIdIt->second.addPosition(latitude, longitude, time);
    notifyObservers(myId);
}

std::list<std::pair<int, Position>> ClientDataManager::getNewPositions(const int &id) {
    std::list<std::pair<int, Position>> newPositions;
    int maxPositionsPerWatcher = 3;

    auto itId = clientInfo.find(id);
    if(itId == clientInfo.end()) {
        std::runtime_error("Error in getNewPositions method. id does not exist");
    }

    for (auto &watcher : itId->second.watchedGroup) {
        auto watcherInfo = clientInfo.at(watcher.id);

        if(!watcherInfo.positions.empty()) {
            std::list<std::pair<int, Position>> newPositionsFromWatcher;
            time_t newTime = 0;

            auto watcherPosition = --watcherInfo.positions.end();
            while (newPositionsFromWatcher.size() < maxPositionsPerWatcher) {
                if(watcherPosition->time > watcher.time) {
                    newPositionsFromWatcher.push_back({watcher.id, *watcherPosition});
                    newTime = std::max(newTime, watcherPosition->time);
                } else {
                    break;
                }
                if(watcherPosition == watcherInfo.positions.begin()) {
                    break;
                }
            }
            watcher.time = std::max(watcher.time, newTime);
            newPositions.insert(newPositions.end(), newPositionsFromWatcher.begin(), newPositionsFromWatcher.end());
        }
    }

    return newPositions;
}

bool ClientDataManager::operator==(const ClientDataManager &rhs) const {
    return nextId == rhs.nextId &&
           clientId == rhs.clientId &&
           clientInfo == rhs.clientInfo;
}

bool ClientDataManager::operator!=(const ClientDataManager &rhs) const {
    return !(rhs == *this);
}
