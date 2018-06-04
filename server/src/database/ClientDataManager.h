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

    short registerClient(const std::vector<unsigned char> &publicKey,
                       message::BlockingQueue<message::Message> *blockingQueue);
    void unregister(const short &id);

    void addObserver(const short &myId, const short &observerId);
    void eraseObserver(const short &myId, const short &observerId);

    void eraseWatcher(const short &myId, const short &watcherId);

    void addPosition(const short &myId, const float &latitude, const float &longitude, const time_t &time);
    std::list<std::pair<short, Position>> getNewPositions(const short &id);

    short getNameId(const std::string &name);
    bool setName(const short &myId, const std::string &name);
    std::list<std::pair<short, std::string>> getNewNames(const short &id);


    bool operator==(const ClientDataManager &rhs) const;
    bool operator!=(const ClientDataManager &rhs) const;

    friend class cereal::access;

protected:
    short nextId = 1;
    std::unordered_map<std::string, short> clientId;
    std::unordered_map<std::string, short> clientName;
    std::unordered_map<short, ClientInfo> clientInfo;
    std::unordered_map<short, message::BlockingQueue<message::Message> *> clientsQueue;

    template<class Archive>
    void serialize(Archive &ar) {
        ar(CEREAL_NVP(nextId), CEREAL_NVP(clientId), CEREAL_NVP(clientName), CEREAL_NVP(clientInfo));
    }

    short addClient(const std::vector<unsigned char> &publicKey);
    void notifyObservers(const short &id);
};


#endif //LOCUS_CLIENTPOSITIONMANAGER_H
