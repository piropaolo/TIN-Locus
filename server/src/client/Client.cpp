#include "Client.h"
#include "log/Logger.h"

using namespace log;
using namespace packet;

void Client::sendPacket(const Packet &packet) {
    auto buffer = packet.getBuffer();
    sendData(buffer.pop(buffer.size()));;
}

packet::Packet Client::recvPacket() {
    packet::Packet packet;
    packet.getBuffer().push_back(recvData());
    return packet;
}
