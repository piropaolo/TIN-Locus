#ifndef LOCUS_MESSAGEQUEUE_H
#define LOCUS_MESSAGEQUEUE_H

#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace message {

    template<class T>
    class BlockingQueue {
    public:
        explicit BlockingQueue(const size_t &maxQueueSize);

        void push(T &&elem);

        bool priorityPush(T &&elem);

        T pop();

        template<class Rep, class Period = std::ratio<1>>
        T pop_for(std::chrono::duration<Rep, Period> real_time);

        const size_t size() const;

        bool empty() const;

    private:
        std::queue<T> queue;
        std::set<T> priority;
        mutable std::mutex accessMutex;
        mutable std::condition_variable emptyCondition;
        mutable std::condition_variable fullCondition;
        const size_t maxQueueSize = 0;

        T popWithoutBlocking();
    };


    template<class T>
    BlockingQueue<T>::BlockingQueue(const size_t &maxQueueSize) : maxQueueSize(maxQueueSize) {
        //check if set working
    }

    template<class T>
    void BlockingQueue<T>::push(T &&elem) {
        {
            std::unique_lock<std::mutex> uniqueGuard(accessMutex);
            fullCondition.wait(uniqueGuard, [&] { return queue.size() < maxQueueSize; });
            queue.push(std::forward<T>(elem));
        }
        emptyCondition.notify_one();
    }

    template<class T>
    bool BlockingQueue<T>::priorityPush(T &&elem) {
        bool result;
        {
            std::lock_guard<std::mutex> guard(accessMutex);
            result = priority.insert(std::forward<T>(elem)).second;
        }
        emptyCondition.notify_one();
        return result;
    }

    template<class T>
    T BlockingQueue<T>::popWithoutBlocking() {
        T ret;
        if (!priority.empty()) {
            ret = *priority.begin();
            priority.erase(priority.begin());
        } else {
            ret = queue.front();
            queue.pop();
        }
        return ret;
    }

    template<class T>
    T BlockingQueue<T>::pop() {
        T ret;
        {
            std::unique_lock<std::mutex> uniqueGuard(accessMutex);
            emptyCondition.wait(uniqueGuard, [&] { return !(queue.empty() && priority.empty()); } );
            ret = popWithoutBlocking();
        }
        fullCondition.notify_one();
        return ret;
    }

    template<class T>
    template<class Rep, class Period>
    T BlockingQueue<T>::pop_for(std::chrono::duration<Rep, Period> real_time) {
        T ret;
        bool notify = false;
        {
            std::unique_lock<std::mutex> uniqueGuard(accessMutex);
            if(emptyCondition.wait_for(uniqueGuard, real_time, [&] { return !(queue.empty() && priority.empty()); } )) {
                ret = popWithoutBlocking();
                notify = true;
            }
        }
        if(notify) {
            fullCondition.notify_one();
        }
        return ret;
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
}


#endif //LOCUS_MESSAGEQUEUE_H
