#include "Client.h"
#include "log/Logger.h"

using namespace Log;
using namespace packet;

void Client::sendPacket(const Packet &packet) {
    auto newPacket = packet;
    newPacket.addTypeToBuffer();
    sendData(newPacket.getBuffer().popAll());;
}

packet::Packet Client::recvPacket() {
    packet::Packet packet;
    packet.getBuffer().push_back(recvData());
    packet.parse();
    return packet;
}
