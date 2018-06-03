#include "Watcher.h"

Watcher::Watcher(int id, time_t time)
        : id(id), time(time) {}

bool Watcher::operator==(const Watcher &rhs) const {
    return id == rhs.id &&
           time == rhs.time;
}

bool Watcher::operator!=(const Watcher &rhs) const {
    return !(rhs == *this);
}

bool Watcher::operator<(const Watcher &rhs) const {
    if (id < rhs.id)
        return true;
    if (rhs.id < id)
        return false;
    return time < rhs.time;
}

bool Watcher::operator>(const Watcher &rhs) const {
    return rhs < *this;
}

bool Watcher::operator<=(const Watcher &rhs) const {
    return !(rhs < *this);
}

bool Watcher::operator>=(const Watcher &rhs) const {
    return !(*this < rhs);
}