#ifndef LOCUS_CLIENTPOSITIONMANAGER_H
#define LOCUS_CLIENTPOSITIONMANAGER_H

#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

#include "ClientInfo.h"
#include "message/Message.h"
#include "message/BlockingQueue.h"

class ClientDataManager {
public:
    ClientDataManager() = default;

    int registerClient(const std::vector<unsigned char> &publicKey,
                       message::BlockingQueue<message::Message> *blockingQueue);
    void unregister(const int &id);

    void addObserver(const int &myId, const int &observerId);
    void eraseObserver(const int &myId, const int &observerId);

    void eraseWatcher(const int &myId, const int &watcherId);

    void addPosition(const int &myId, const float &latitude, const float &longitude, const time_t &time);

    std::list<std::pair<int, Position>> getNewPositions(const int &id);

    bool operator==(const ClientDataManager &rhs) const;
    bool operator!=(const ClientDataManager &rhs) const;

    friend class cereal::access;

protected:
    int nextId = 0;
    std::unordered_map<std::string, int> clientId;
    std::unordered_map<int, ClientInfo> clientInfo;
    std::unordered_map<int, message::BlockingQueue<message::Message> *> clientsQueue;

    template<class Archive>
    void serialize(Archive &ar) {
        ar(CEREAL_NVP(nextId), CEREAL_NVP(clientId), CEREAL_NVP(clientInfo));
    }

    int addClient(const std::vector<unsigned char> &publicKey);
    void notifyObservers(const int &id);
};


#endif //LOCUS_CLIENTPOSITIONMANAGER_H
