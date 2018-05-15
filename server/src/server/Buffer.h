#ifndef LOCUS_BUFFER_H
#define LOCUS_BUFFER_H

#include <cstddef>
#include <vector>
#include <list>
#include <mutex>
#include <utility>

//namespace {
class Buffer {
public:
    enum class Stage {
        Empty,
        Full,
        Receiving,
        Sending
    };

    explicit Buffer() = default;

    void setMaxSize(const size_t &bufferMaxSize);

    const size_t size() const;

    const size_t remainingSize() const;

    const bool empty() const;

    const bool full() const;

    Stage getStage() const;

    void setStage(const Stage &stage);

//        void push_front(const std::vector<std::byte> &bytes);
//        void push_front(std::vector<std::byte> &&bytes);
    template<typename T>
    void push_front(T &&bytes);


//        void push_back(const std::vector<std::byte> &bytes);
//        void push_back(std::vector<std::byte> &&bytes);
    template<typename T>
    void push_back(T &&bytes);

    std::vector<std::byte> pop();

    std::vector<std::byte> pop(const size_t &n);

private:
    std::mutex accessMutex;
    std::list<std::vector<std::byte> > buffer;
    size_t bufferMaxSize = 0;
    size_t bufferSize = 0;
    Stage stage = Stage::Empty;

    const size_t frontSize() const;
};

template<typename T>
void Buffer::push_front(T &&bytes) {
    static_assert(std::is_same<std::vector<std::byte>, typename std::remove_reference<T>::type>::value,
                  "Parameter in push_front must be std::vector<std::byte> type");

    std::lock_guard<std::mutex> guard(accessMutex);
    bufferSize += bytes.size();
    buffer.push_front(std::forward<std::vector<std::byte> >(bytes));
}

template<typename T>
void Buffer::push_back(T &&bytes) {
    static_assert(std::is_same<std::vector<std::byte>, typename std::remove_reference<T>::type>::value,
                  "Parameter in push_back must be std::vector<std::byte> type");

    std::lock_guard<std::mutex> guard(accessMutex);
    bufferSize += bytes.size();
    buffer.push_back(std::forward<T>(bytes));
}
//}


#endif //LOCUS_BUFFER_H
