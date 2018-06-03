#ifndef LOCUS_WATCHER_H
#define LOCUS_WATCHER_H

#include <cereal/cereal.hpp>
#include <ctime>

struct Watcher {
    Watcher() = default;
    Watcher(int id, time_t time);

    int id = 0;
    time_t time = 0;

    template<class Archive>
    void serialize(Archive &ar) {
        ar(CEREAL_NVP(id), CEREAL_NVP(time));
    }

    bool operator==(const Watcher &rhs) const;
    bool operator!=(const Watcher &rhs) const;

    bool operator<(const Watcher &rhs) const;
    bool operator>(const Watcher &rhs) const;
    bool operator<=(const Watcher &rhs) const;
    bool operator>=(const Watcher &rhs) const;
};


#endif //LOCUS_WATCHER_H
