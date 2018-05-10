#include <cstring>
#include <cerrno>
#include <unistd.h>

#include "ClientBuffer.h"
#include "log/Logger.h"

using namespace log;

ClientBuffer::ClientBuffer(const int &port, const sockaddr &client_addr)
        : client_addr(client_addr), bufferIn(1000), bufferOut(1000) {
    setFileDescriptor(port);
}

const bool ClientBuffer::isClose() const {
    return close;
}

const sockaddr &ClientBuffer::getClient_addr() const {
    return client_addr;
}

Buffer &ClientBuffer::getBufferIn() {
    return bufferIn;
}

Buffer &ClientBuffer::getBufferOut() {
    return bufferOut;
}

void ClientBuffer::recvData() {
    size_t countSum = 0;
    size_t maxSize = 512;
    std::vector<std::byte> buffer(maxSize);
    while (!isClose() && bufferIn.full()) {
        ssize_t count;

        count = recv(getFileDescriptor(), buffer.data(), sizeof(char) * maxSize, 0);
        if (count < 0) {
            // if errno is EAGAIN or EWOULDBLOCK, that means we have receive all data.
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                close = true;
                Logger::getInstance().logError("ClientBuffer: Error while recv: "
                                               + std::string(std::strerror(static_cast<int>(count))));
            }
            Logger::getInstance().logMessage("ClientBuffer: receive " + std::to_string(countSum) + " bytes.");
            return;
        } else if (count == 0) {
            // End of file. The remote has close the connection.
            close = true;
            Logger::getInstance().logMessage("ClientBuffer: The remote has close the connection");
            return;
        }
        // receive count bytes
        countSum += count;
        buffer.resize(static_cast<size_t>(count));
        if (!bufferIn.push_back(buffer)) {
            Logger::getInstance().logDebug("ClientBuffer: bufferIn is full.");
            return;
        }
        buffer.resize(maxSize);

        //for now print on terminal
        write(1, buffer.data(), static_cast<size_t>(count));
        write(1, "\n", 1);
    }
}

void ClientBuffer::sendData() {
    size_t countSum = 0;
    while (!isClose() && bufferOut.size()) {
        ssize_t count;
        std::vector<std::byte> buffer = bufferOut.pop();

        count = send(getFileDescriptor(), buffer.data(), sizeof(char) * buffer.size(), 0);
        if (count < 0) {
            // if errno is EAGAIN or EWOULDBLOCK, that means we have send all data.
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                close = true;
                Logger::getInstance().logError("ClientBuffer: Error while send:"
                                               + std::string(std::strerror(static_cast<int>(count))));
            }
            Logger::getInstance().logMessage("ClientBuffer: send " + std::to_string(countSum) + " bytes.");
            return;
        } else if (count == 0) {
            Logger::getInstance().logDebug("ClientBuffer: send return 0, why?");
            return;
        }
        // receive count bytes
        countSum += count;

        //store reminded bytes
        if(buffer.size() < count) {
            buffer.erase(buffer.begin(), buffer.begin() + count);
            bufferOut.push_front(std::move(buffer));
        }
    }
}
