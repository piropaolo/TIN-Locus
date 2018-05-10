#include "BufferManager.h"
#include <stdexcept>

//namespace {

//    void BufferManager::add(const int &socketID) {
//        buffersMap.insert({socketID, Buffer(buffersSize)});
//    }
//
//    void BufferManager::erase(const int &socketID) {
//        buffersMap.erase(socketID);
//    }
//
//    Buffer *BufferManager::getBufferIn(const int &socketID) {
//        auto buffer = buffersMap.find(socketID);
//        if (buffer == buffersMap.end()) {
//            throw std::runtime_error("Can't find the buffer");
//        }
//        return &buffer->second;
//    }
//
//    bool BufferManager::push_back(const int &socketID, const std::vector<std::byte> &bytes) {
//        auto buffer = buffersMap.find(socketID);
//        if (buffer == buffersMap.end()) {
//            throw std::runtime_error("Can't find the buffer");
//        }
//        buffer->second.push_back(bytes);
//    }
//}