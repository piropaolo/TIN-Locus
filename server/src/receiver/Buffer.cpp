#include "Buffer.h"
#include <stdexcept>

//namespace {
Buffer::Buffer(const size_t &maxSize) : bufferMaxSize(maxSize) {}

const size_t Buffer::size() const {
    return bufferSize;
}

const bool Buffer::full() const {
    return bufferSize >= bufferMaxSize;
}

bool Buffer::push_front(const std::vector<std::byte> &bytes) {
    std::lock_guard<std::mutex> guard(accesMutex);
    bufferSize += bytes.size();
    buffer.push_front(bytes);

    return !full();
}

bool Buffer::push_front(std::vector<std::byte> &&bytes) {
    std::lock_guard<std::mutex> guard(accesMutex);
    bufferSize += bytes.size();
    buffer.push_front(std::move(bytes));

    return !full();
}

bool Buffer::push_back(const std::vector<std::byte> &bytes) {
    std::lock_guard<std::mutex> guard(accesMutex);
    bufferSize += bytes.size();
    buffer.push_back(bytes);

    return !full();
}

bool Buffer::push_back(std::vector<std::byte> &&bytes) {
    std::lock_guard<std::mutex> guard(accesMutex);
    bufferSize += bytes.size();
    buffer.push_back(std::move(bytes));

    return !full();
}

std::vector<std::byte> Buffer::pop(const size_t &n) {
    if (n > size()) {
        throw std::runtime_error("The Buffer has too small number of bytes");
    }

    std::lock_guard<std::mutex> guard(accesMutex);
    std::vector<std::byte> returnBuffer(n);
    auto itRetBuffer = returnBuffer.begin();
    size_t left = n;


    while (left) {
        auto itBuffer = buffer.front().begin();
        if (frontSize() <= left) {
            for (int i = 0; i < frontSize(); ++i) {
                *itRetBuffer++ = *itBuffer++;
            }
            left -= frontSize();
            buffer.pop_front();
        } else {
            for (int i = 0; i < left; ++i) {
                *itRetBuffer++ = *itBuffer++;
            }
            left = 0;
            buffer.front().erase(buffer.front().begin(), itBuffer);
        }
    }

    bufferSize -= n;
    return returnBuffer;
}

std::vector<std::byte> Buffer::pop() {
    if (!size()) {
        throw std::runtime_error("The Buffer is empty");
    }

    std::lock_guard<std::mutex> guard(accesMutex);
    std::vector<std::byte> bytes = std::move(buffer.front());
    buffer.pop_front();
    bufferSize -= bytes.size();
    return std::move(bytes);
}

const size_t Buffer::frontSize() const {
    return buffer.front().size();
}
//}