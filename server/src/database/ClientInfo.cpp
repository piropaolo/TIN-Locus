#include "ClientInfo.h"
#include "buffer/Converter.h"

using namespace buffer;

ClientInfo::ClientInfo(const std::vector<unsigned char> &publicKey)
        : publicKey(publicKey) {
}

void ClientInfo::setName(const std::string &name) {
    this->name = name;
}

void ClientInfo::addPosition(const float &latitude, const float &longitude, const time_t &time) {
    positions.emplace_back(Position(latitude, longitude, time));
}

void ClientInfo::addObserver(const short &id) {
    observers.push_back(id);
}

void ClientInfo::eraseObserver(const short &id) {
    observers.remove(id);
}

void ClientInfo::addWatcher(const short &id) {
    watchedGroup.emplace_back(Watcher(id));
}

void ClientInfo::eraseWatcher(const short &id) {
    watchedGroup.remove_if([&id](Watcher &w) { return w.id == id; });
}

std::vector<unsigned char> ClientInfo::getPublicKey() const {
    return publicKey;
}

bool ClientInfo::operator==(const ClientInfo &rhs) const {
    return publicKey == rhs.publicKey &&
           positions == rhs.positions &&
           observers == rhs.observers &&
           watchedGroup == rhs.watchedGroup;
}

bool ClientInfo::operator!=(const ClientInfo &rhs) const {
    return !(rhs == *this);
}

bool ClientInfo::operator<(const ClientInfo &rhs) const {
    if (publicKey < rhs.publicKey)
        return true;
    if (rhs.publicKey < publicKey)
        return false;
    if (name < rhs.name)
        return true;
    if (rhs.name < name)
        return false;
    if (positions < rhs.positions)
        return true;
    if (rhs.positions < positions)
        return false;
    if (observers < rhs.observers)
        return true;
    if (rhs.observers < observers)
        return false;
    return watchedGroup < rhs.watchedGroup;
}

bool ClientInfo::operator>(const ClientInfo &rhs) const {
    return rhs < *this;
}

bool ClientInfo::operator<=(const ClientInfo &rhs) const {
    return !(rhs < *this);
}

bool ClientInfo::operator>=(const ClientInfo &rhs) const {
    return !(*this < rhs);
}
