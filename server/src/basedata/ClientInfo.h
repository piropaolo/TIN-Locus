#ifndef LOCUS_CLIENTINFO_H
#define LOCUS_CLIENTINFO_H

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/string.hpp>
#include <ctime>

#include "Position.h"
#include "Watcher.h"

struct ClientInfo {
    ClientInfo() = default;
    explicit ClientInfo(const std::vector<unsigned char> &publicKey);

    std::vector<unsigned char> publicKey;
    std::string name;
    std::list<Position> positions; //x, y, time
    std::list<int> observers; //i'm sending the location
    std::list<Watcher> watchedGroup; //they are sending the location

//    friend class cereal::access;

    template<class Archive>
    void serialize(Archive &ar) {
        ar(CEREAL_NVP(publicKey), CEREAL_NVP(name), CEREAL_NVP(positions), CEREAL_NVP(observers), CEREAL_NVP(watchedGroup));
    }

    void setName(const std::string &name);

    void addPosition(const float &latitude, const float &longitude, const time_t &time);

    void addObserver(const int &id);
    void eraseObserver(const int &id);

    void addWatcher(const int &id);
    void eraseWatcher(const int &id);

    bool operator==(const ClientInfo &rhs) const;
    bool operator!=(const ClientInfo &rhs) const;

    bool operator<(const ClientInfo &rhs) const;
    bool operator>(const ClientInfo &rhs) const;
    bool operator<=(const ClientInfo &rhs) const;
    bool operator>=(const ClientInfo &rhs) const;
};


#endif //LOCUS_CLIENTINFO_H
