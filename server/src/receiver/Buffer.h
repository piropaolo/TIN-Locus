#ifndef LOCUS_BUFFER_H
#define LOCUS_BUFFER_H

#include <cstddef>
#include <vector>
#include <list>
#include <mutex>

//namespace {
    class Buffer {
    public:
        explicit Buffer(const size_t &maxSize);

        const size_t size() const;
        const bool full() const;

        bool push_front(const std::vector<std::byte> &bytes);
        bool push_front(std::vector<std::byte> &&bytes);

        bool push_back(const std::vector<std::byte> &bytes);
        bool push_back(std::vector<std::byte> &&bytes);

        std::vector<std::byte> pop();
        std::vector<std::byte> pop(const size_t &n);

    private:
        std::mutex accesMutex;
        std::list< std::vector<std::byte> > buffer;
        size_t bufferMaxSize;
        size_t bufferSize = 0;

        const size_t frontSize() const;
    };
//}


#endif //LOCUS_BUFFER_H
