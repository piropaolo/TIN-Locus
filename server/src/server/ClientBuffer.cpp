#include <cstring>
#include <cerrno>
#include <unistd.h>

#include "ClientBuffer.h"
#include "log/Logger.h"
#include "packet/Converter.h"

using namespace log;
using namespace packet;
using namespace message;

ClientBuffer::ClientBuffer(const int &port) {
    setFileDescriptor(port);
}

const bool ClientBuffer::isClose() const {
    return close;
}

Buffer &ClientBuffer::getBufferIn() {
    return bufferIn;
}

Buffer &ClientBuffer::getBufferOut() {
    return bufferOut;
}

std::vector<std::byte> ClientBuffer::recvData(const size_t &n) const {
    std::vector<std::byte> buffer(n);
    size_t remaining = n;

    while (!isClose() && remaining) {
        buffer.resize(remaining);
        ssize_t count = recv(getFileDescriptor(), buffer.data() + n - remaining, sizeof(std::byte) * remaining, 0);

        if (count < 0) {
            // if errno is EAGAIN or EWOULDBLOCK, that means we have receive all data.
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                close = true;
                Logger::getInstance().logError("ClientBuffer: Error while recv: " + std::string(std::strerror(errno)));
            }
            break;
        } else if (count == 0) {
            // End of file. The remote has close the connection.
            close = true;
            Logger::getInstance().logMessage("ClientBuffer: " + std::to_string(getFileDescriptor()) + 
                                             " has close the connection");
            break;
        }

        // receive count bytes
        remaining -= count;
    }

    if(!isClose()) {
        Logger::getInstance().logDebug("ClientBuffer: receive " + std::to_string(n - remaining) + " bytes.");
    }

    buffer.resize(n - remaining);
    return isClose() ? std::vector<std::byte>() : buffer;
}

void ClientBuffer::recvData() {

    //get size of upcoming packet
    if (!isClose() && bufferIn.getStage() == Buffer::Stage::Empty) {
        auto buffer = recvData(1);
        if (!buffer.empty()) {
            uint8_t size = 0;
            from_bytes(buffer, size);
            bufferIn.setMaxSize(size);
            bufferIn.setStage(Buffer::Stage::Receiving);
        }
    }

    //Receiving data
    if (!isClose() && bufferIn.getStage() == Buffer::Stage::Receiving) {
        auto buffer = recvData(bufferIn.remainingSize());
        if (!buffer.empty()) {
            bufferIn.push_back(std::move(buffer));
            if (bufferIn.remainingSize() == 0) {
                bufferIn.setStage(Buffer::Stage::Full);
                //notify client
                if(clientBlockingQueue) {
                    clientBlockingQueue->priorityPush(Message(Message::PacketReceive));
                }
            }
        }
    }

    //notify client if connection is closed
    if(isClose() && clientBlockingQueue) {
        clientBlockingQueue->priorityPush(Message(Message::Close));
    }
}

std::vector<std::byte> ClientBuffer::sendData(std::vector<std::byte> buffer) const {
    Logger::getInstance().logDebug("ClientBuffer: try to send " + std::to_string(buffer.size()) + " bytes.");

    const auto firstSize = buffer.size();
    if (!isClose() && !buffer.empty()) {
        ssize_t count = send(getFileDescriptor(), buffer.data(), sizeof(std::byte) * buffer.size(), 0);
        if (count < 0) {
            // if errno is EAGAIN or EWOULDBLOCK, that means we have send all data.
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                close = true;
                Logger::getInstance().logError("ClientBuffer: Error while send:" + std::string(std::strerror(errno)));
            }
        } else if (count == 0) {
            Logger::getInstance().logDebug("ClientBuffer: send return 0, why?");
        } else {
            // receive count bytes
            buffer.erase(buffer.begin(), buffer.begin() + count);
        }
    }
    
    if(!isClose()) {
        Logger::getInstance().logDebug("ClientBuffer: send " + std::to_string(firstSize - buffer.size()) + " bytes.");
    }
    
    return buffer;
}

void ClientBuffer::sendData() {
    //set size of sending packet
    if (!isClose() && bufferOut.getStage() == Buffer::Stage::Full) {
        const auto size = static_cast<uint8_t>(bufferOut.size());
        bufferOut.push_front(to_bytes(size));
        bufferOut.setMaxSize(bufferOut.size());
        bufferOut.setStage(Buffer::Stage::Sending);
    }

    //Sending data
    while (!isClose() && bufferOut.getStage() == Buffer::Stage::Sending) {
        auto buffer = sendData(bufferOut.pop());
        if (!buffer.empty()) {
//            Logger::getInstance().logDebug("ClientBuffer: remained " + std::to_string(buffer.size()) + " bytes.");
            bufferOut.push_front(std::move(buffer));
            break;
        } else if (bufferOut.empty()) {
//            Logger::getInstance().logDebug("ClientBuffer: bufferOut is empty");
            bufferOut.setStage(Buffer::Stage::Empty);
            //notify client
            if(clientBlockingQueue) {
                clientBlockingQueue->priorityPush(Message(Message::PacketSend));
            }
            break;
        }
    }
}

void ClientBuffer::setClientBlockingQueue(BlockingQueue<Message> *clientBlockingQueue) {
    ClientBuffer::clientBlockingQueue = clientBlockingQueue;
}
