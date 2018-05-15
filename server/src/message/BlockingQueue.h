#ifndef LOCUS_MESSAGEQUEUE_H
#define LOCUS_MESSAGEQUEUE_H

#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <memory>

#include <unistd.h> // pipe
#include <fcntl.h>  // O_NONBLOCK flag for pipe
#include <cerrno>
#include <system_error>

namespace message {

    template<class T>
    class BlockingQueue {
    public:
        explicit BlockingQueue(const size_t &maxQueueSize);

        BlockingQueue(BlockingQueue &&rth) noexcept;

        void push(T &&elem);

        bool priorityPush(T &&elem);

        T pop();

        template<class Rep, class Period = std::ratio<1>>
        T pop_for(std::chrono::duration<Rep, Period> real_time);

        const size_t size() const;

        bool empty() const;

        int getReadFd() const;
        int getWriteFd() const;

    private:
        std::queue<T> queue;
        std::set<std::unique_ptr<T> > priority;
        mutable std::mutex accessMutex;
        mutable std::condition_variable emptyCondition;
        mutable std::condition_variable fullCondition;
        const size_t maxQueueSize = 0;

        /*mutable*/ union {
            struct {
                int pipeReadFd;
                int pipeWriteFd;
            };
            int pipeFds[2];
        };

        void pushByteToPipe() const;
        void popByteFromPipe() const;

        T popWithoutBlocking();
    };


    template<class T>
    BlockingQueue<T>::BlockingQueue(const size_t &maxQueueSize)
            : maxQueueSize(maxQueueSize) {
        //check if set working

        // open pipe, NOTE: pipe2 is Linux-specific
        if (pipe2(pipeFds, O_NONBLOCK | O_CLOEXEC) != 0) {
            throw std::system_error(errno, std::generic_category());
        }

        // close pipe fds in destructor? O_CLOEXEC flag?
    }

    template<class T>
    BlockingQueue<T>::BlockingQueue(BlockingQueue &&rth) noexcept
            : maxQueueSize(rth.maxQueueSize) {
        std::lock_guard<std::mutex> guard(rth.accessMutex);
        queue = std::move(rth.queue);
        priority = std::move(rth.priority);

        // open pipe, NOTE: pipe2 is Linux-specific
        if (pipe2(pipeFds, O_NONBLOCK | O_CLOEXEC) != 0) {
            throw std::system_error(errno, std::generic_category());
        }
    }

    template<class T>
    void BlockingQueue<T>::push(T &&elem) {
        {
            std::unique_lock<std::mutex> uniqueGuard(accessMutex);
            fullCondition.wait(uniqueGuard, [&] { return queue.size() < maxQueueSize; });
            queue.push(std::forward<T>(elem));
            pushByteToPipe();
        }
        emptyCondition.notify_one();
    }

    template<class T>
    bool BlockingQueue<T>::priorityPush(T &&elem) {
        bool result;
        {
            std::lock_guard<std::mutex> guard(accessMutex);
            result = priority.insert(std::make_unique<T>(std::forward<T>(elem))).second;
            pushByteToPipe();
        }
        emptyCondition.notify_one();
        return result;
    }

    template<class T>
    T BlockingQueue<T>::popWithoutBlocking() {
        T ret;
        if (!priority.empty()) {
            ret = std::move(*priority.begin()->get());
            priority.erase(priority.begin());
        } else {
            ret = std::move(queue.front());
            queue.pop();
        }
        popByteFromPipe();
        return std::move(ret);
    }

    template<class T>
    T BlockingQueue<T>::pop() {
        T ret;
        {
            std::unique_lock<std::mutex> uniqueGuard(accessMutex);
            emptyCondition.wait(uniqueGuard, [&] { return !(queue.empty() && priority.empty()); });
            ret = popWithoutBlocking();
        }
        fullCondition.notify_one();
        return std::move(ret);
    }

    template<class T>
    template<class Rep, class Period>
    T BlockingQueue<T>::pop_for(std::chrono::duration<Rep, Period> real_time) {
        T ret;
        bool notify = false;
        {
            std::unique_lock<std::mutex> uniqueGuard(accessMutex);
            if (emptyCondition.wait_for(uniqueGuard, real_time, [&] { return !(queue.empty() && priority.empty()); })) {
                ret = popWithoutBlocking();
                notify = true;
            }
        }
        if (notify) {
            fullCondition.notify_one();
        }
        return std::move(ret);
    }

    template<class T>
    const size_t BlockingQueue<T>::size() const {
        std::lock_guard<std::mutex> guard(accessMutex);
        return queue.size() + priority.size();
    }

    template<class T>
    bool BlockingQueue<T>::empty() const {
        std::lock_guard<std::mutex> guard(accessMutex);
        return queue.empty() && priority.empty();
    }

    template<class T>
    void BlockingQueue<T>::pushByteToPipe() const {
        // std::byte ?
        if (write(pipeWriteFd, "1", sizeof "1"[0]) == -1) {
            throw std::system_error(errno, std::generic_category());
        }

        // something more?
    }

    template<class T>
    void BlockingQueue<T>::popByteFromPipe() const {
        unsigned char byte;
        if (read(pipeReadFd, (void*)&byte, sizeof byte) == -1) {
            throw std::system_error(errno, std::generic_category());
        }

        // something more?
    }

    template<class T>
    int BlockingQueue<T>::getReadFd() const {
        return pipeReadFd;
    }

    template<class T>
    int BlockingQueue<T>::getWriteFd() const {
        return pipeWriteFd;
    }
}


#endif //LOCUS_MESSAGEQUEUE_H
