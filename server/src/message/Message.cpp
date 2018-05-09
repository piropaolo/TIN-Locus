#include "Message.h"

using namespace packet;

namespace message {
    Message::Message(const Message::Type &type) : type(type) {}

    Message::Message(const ErrorType &errorType)
            : type(Type::Error), errorType(errorType) {}

    Message::Message(const packet::Packet &packetMsg)
            : type(Type::Packet), packetMsg(packetMsg) {}

    Message::Message(packet::Packet &&packetMsg)
            : type(Type::Packet), packetMsg(std::move(packetMsg)) {}

    Message::Type Message::getType() const {
        return type;
    }

    const ErrorType &Message::getErrorType() const {
        return errorType;
    }

    const Packet &Message::getPacketMsg() const {
        return packetMsg;
    }

    bool Message::operator==(const Message &rhs) const {
        return type == rhs.type &&
               errorType == rhs.errorType &&
               packetMsg == rhs.packetMsg;
    }

    bool Message::operator!=(const Message &rhs) const {
        return !(rhs == *this);
    }

    bool Message::operator<(const Message &rhs) const {
        if (type < rhs.type)
            return true;
        if (rhs.type < type)
            return false;
        if (errorType < rhs.errorType)
            return true;
        if (rhs.errorType < errorType)
            return false;
        return packetMsg < rhs.packetMsg;
    }

    bool Message::operator>(const Message &rhs) const {
        return rhs < *this;
    }

    bool Message::operator<=(const Message &rhs) const {
        return !(rhs < *this);
    }

    bool Message::operator>=(const Message &rhs) const {
        return !(*this < rhs);
    }
}