#include "ClientDataManager.h"

#include <stdexcept>
#include "buffer/Converter.h"

using namespace buffer;
using namespace message;

short ClientDataManager::addClient(const std::vector<unsigned char> &publicKey) {
    clientId.insert({toString(publicKey), nextId});
    clientInfo.insert({nextId, ClientInfo(publicKey)});
    return nextId++;
}

short ClientDataManager::registerClient(const std::vector<unsigned char> &publicKey,
                                        message::BlockingQueue<message::Message> *blockingQueue) {
    short id = 0;

    auto clientIdIt = clientId.find(toString(publicKey));
    if (clientIdIt != clientId.end()) {
        id = clientIdIt->second;
    } else {
        id = addClient(publicKey);
    }

    clientsQueue.insert({id, blockingQueue});
    return id;
}

void ClientDataManager::unregister(const short &id) {
    clientsQueue.erase(id);
}

void ClientDataManager::addObserver(const short &myId, const short &observerId) {
    auto myIdIt = clientInfo.find(myId);
    if (myIdIt == clientInfo.end()) {
        std::runtime_error("Error in addObserver method. myId does not exist");
    }
    auto observerIdIt = clientInfo.find(observerId);
    if (observerIdIt == clientInfo.end()) {
        std::runtime_error("Error in addObserver method. observerId does not exist");
    }

    myIdIt->second.addObserver(observerId);
    observerIdIt->second.addWatcher(myId);

    auto observerQueue = clientsQueue.find(observerId);
    if (observerQueue != clientsQueue.end()) {
        observerQueue->second->priorityPush(Message(Message::Type::Update));
    }
}

void ClientDataManager::eraseObserver(const short &myId, const short &observerId) {
    auto myIdIt = clientInfo.find(myId);
    if (myIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseObserver method. myId does not exist");
    }
    auto observerIdIt = clientInfo.find(observerId);
    if (observerIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseObserver method. observerId does not exist");
    }

    myIdIt->second.eraseObserver(observerId);
    observerIdIt->second.eraseWatcher(myId);

    auto observerQueue = clientsQueue.find(observerId);
    if (observerQueue != clientsQueue.end()) {
        Message message(Message::Type::EraseObserver);
        message.id = std::make_unique<short>(myId);
        observerQueue->second->priorityPush(std::move(message));
    }
}

void ClientDataManager::eraseWatcher(const short &myId, const short &watcherId) {
    auto myIdIt = clientInfo.find(myId);
    if (myIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseWatcher method. myId does not exist");
    }
    auto watcherIdIt = clientInfo.find(watcherId);
    if (watcherIdIt == clientInfo.end()) {
        std::runtime_error("Error in eraseWatcher method. watcherId does not exist");
    }

    myIdIt->second.eraseWatcher(watcherId);
    watcherIdIt->second.eraseObserver(myId);
}

void ClientDataManager::notifyObservers(const short &id) {
    auto clientInfoIt = clientInfo.find(id);
    if (clientInfoIt == clientInfo.end()) {
        std::runtime_error("Error in notifyObservers method. id does not exist");
    }

    for (auto &observer : clientInfoIt->second.observers) {
        auto observerQueue = clientsQueue.find(observer);
        if (observerQueue != clientsQueue.end()) {
            observerQueue->second->priorityPush(Message(Message::Type::Update));
        }
    }
}

void
ClientDataManager::addPosition(const short &myId, const float &latitude, const float &longitude, const time_t &time) {
    auto myIdIt = clientInfo.find(myId);
    if (myIdIt == clientInfo.end()) {
        std::runtime_error("Error in addPosition method. myId does not exist");
    }

    myIdIt->second.addPosition(latitude, longitude, time);
    notifyObservers(myId);
}

std::list<std::pair<short, Position>> ClientDataManager::getNewPositions(const short &id) {
    std::list<std::pair<short, Position>> newPositions;
    int maxPositionsPerWatcher = 3;

    auto itId = clientInfo.find(id);
    if (itId == clientInfo.end()) {
        std::runtime_error("Error in getNewPositions method. id does not exist");
    }

    for (auto &watcher : itId->second.watchedGroup) {
        auto watcherInfo = clientInfo.at(watcher.id);

        if (!watcherInfo.positions.empty() && !watcherInfo.name.empty()) {
            std::list<std::pair<short, Position>> newPositionsFromWatcher;
            time_t newTime = 0;

            auto watcherPosition = --watcherInfo.positions.end();
            while (newPositionsFromWatcher.size() < maxPositionsPerWatcher) {
                if (watcherPosition->time > watcher.time) {
                    newPositionsFromWatcher.push_back({watcher.id, *watcherPosition});
                    newTime = std::max(newTime, watcherPosition->time);
                } else {
                    break;
                }
                if (watcherPosition == watcherInfo.positions.begin()) {
                    break;
                }
            }
            watcher.time = std::max(watcher.time, newTime);
            newPositions.insert(newPositions.end(), newPositionsFromWatcher.begin(), newPositionsFromWatcher.end());
        }
    }

    return newPositions;
}

short ClientDataManager::getNameId(const std::string &name) {
    auto nameId = clientName.find(name);
    return nameId != clientName.end() ? nameId->second : static_cast<short>(0);
}

bool ClientDataManager::setName(const short &myId, const std::string &name) {
    auto myIdIt = clientInfo.find(myId);
    if (myIdIt == clientInfo.end()) {
        std::runtime_error("Error in addPosition method. myId does not exist");
    }

    auto nameCount = clientName.count(name);
    if (nameCount == 0) {
        if (!myIdIt->second.name.empty()) {
            clientName.erase(myIdIt->second.name);
        }
        clientName.insert({name, myId});
        myIdIt->second.setName(name);
        notifyObservers(myId);
        return true;
    } else {
        return nameCount == 1 && myIdIt->second.name == name;
    }
}

std::list<std::pair<short, std::string>> ClientDataManager::getNewNames(const short &id) {
    std::list<std::pair<short, std::string>> newNames;

    auto itId = clientInfo.find(id);
    if (itId == clientInfo.end()) {
        std::runtime_error("Error in getNewNames method. id does not exist");
    }

    for (auto &watcher : itId->second.watchedGroup) {
        auto watcherInfo = clientInfo.at(watcher.id);

        if (!watcherInfo.name.empty() && watcherInfo.name != watcher.name) {
            watcher.name = watcherInfo.name;
            newNames.push_back({watcher.id, watcherInfo.name});
        }
    }

    return newNames;
}

bool ClientDataManager::operator==(const ClientDataManager &rhs) const {
    return nextId == rhs.nextId &&
           clientId == rhs.clientId &&
           clientInfo == rhs.clientInfo;
}

bool ClientDataManager::operator!=(const ClientDataManager &rhs) const {
    return !(rhs == *this);
}
