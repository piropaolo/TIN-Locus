#ifndef LOCUS_MESSAGE_H
#define LOCUS_MESSAGE_H

#include <memory>
#include "ErrorType.h"
#include "packet/Packet.h"

namespace message {
    class Message {
    public:
        enum Type : unsigned int {
            Empty,
            Error,
            Close,
            Packet
        };

        Message() = default;
        explicit Message(const Type &type);
        explicit Message(const ErrorType &errorType);
        explicit Message(const packet::Packet &packetMsg);
        explicit Message(packet::Packet &&packetMsg);

        Type getType() const;
        const ErrorType &getErrorType() const;
        const packet::Packet &getPacketMsg() const;

        bool operator==(const Message &rhs) const;
        bool operator!=(const Message &rhs) const;

        bool operator<(const Message &rhs) const;
        bool operator>(const Message &rhs) const;
        bool operator<=(const Message &rhs) const;
        bool operator>=(const Message &rhs) const;

    private:
        Type type = Empty;
        ErrorType errorType = ErrorType::Undefined;
        packet::Packet packetMsg = packet::Packet(packet::PacketType::UNDEFINED);
    };
}


#endif //LOCUS_MESSAGE_H
