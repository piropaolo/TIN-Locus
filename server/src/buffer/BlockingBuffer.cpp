#include "BlockingBuffer.h"
#include <stdexcept>

//namespace {

std::vector<std::byte> BlockingBuffer::pop(const size_t &n) {
    std::lock_guard<std::mutex> guard(accessMutex);
    return Buffer::pop(n);
}

std::vector<std::byte> BlockingBuffer::pop() {
    std::lock_guard<std::mutex> guard(accessMutex);
    return Buffer::pop();
}


//}