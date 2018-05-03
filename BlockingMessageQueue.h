#ifndef BLOCKING_MESSAGE_QUEUE_H
#define BLOCKING_MESSAGE_QUEUE_H

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
  #include <pthread.h>
}

#include <queue>

namespace messages
{
    struct Message
    {
        /*const*/ char *buffer; // TODO? void* ?
        std::size_t size;

        Message(const char *msg_buffer, std::size_t msg_size); // TODO? automatic buffer alloc? (messages travel between threads, y'know)
        // TODO? move ctor??
        /*explicit*/ Message(const Message &message);
        /*Message& operator=(const Message &message);*/
        ~Message()/* = default*/;   // TODO? automatic buffer alloc?
    };
}

namespace threads_layer
{
    // NOTE: must be ref-counted (?)
    class BlockingMessageQueue
    {
        private:
          std::queue<messages::Message> queue_;
          // or: /*const*/ Message& ??

          // mutable - applies to non-static class members of non-reference non-const type and specifies that the member does not affect the externally visible state of the class (as often used for mutexes, memo caches, lazy evaluation, and access instrumentation). mutable members of const class instances are modifiable. (Note: the C++ language grammar treats mutable as a storage-class-specifier, but it does not affect storage class.)
          mutable pthread_mutex_t access_mutex_;
          mutable pthread_cond_t empty_cond_;

          //std::size_t size_; // TODO?
          //const std::size_t MAX_SIZE_ = 10; // TODO?

        public:
          BlockingMessageQueue();
          ~BlockingMessageQueue();

          // TODO? move instead of copy?
          void enqueue(const messages::Message &message); // should never block on condition "full"
          messages::Message dequeue();    // TODO? reference?

          // TODO? emplace
          //void enqueue(const char *buffer, std::size_t size);

          // TODO? or:
          // Inside sending thread:
          // 1. blockingMessageQueue.waitForMessage(); // mutex lock
          // 2. blockingMessageQueue.getMessageRef();
          // 3. send using message in blockingMessageQueue's buffer
          // 4. blockingMessageQueue.removeMessage();
          // 5. blockingMessageQueue.done(); // mutex unlock
          // or some RAII mutex ^
          // thus no need to copy message just for sending
          // BUT sending can sometimes block for some time

          // ref-counted messages in some external buffer, only references in queue? => but limit on number of messages (?)
    };
}

#endif /*BLOCKING_MESSAGE_QUEUE_H*/