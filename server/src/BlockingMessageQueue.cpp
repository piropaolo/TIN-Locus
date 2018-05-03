#include "BlockingMessageQueue.h"

// NOTE: C++11 required
#include <system_error>

#include <stdexcept>

// DBG:
#include <iostream>
#include <cstring>  // std::strerror, std::memcpy

#include <utility>

namespace messages
{
    // RAII
    Message::Message(const char *msg_buffer, std::size_t msg_size) /*: buffer(msg_buffer), size(msg_size)*/
    {
        if( msg_buffer == nullptr )
            throw std::invalid_argument("Invalid message buffer address: nullptr.");
            
        buffer = new char[msg_size];
        //  > check errors (std::bad_alloc exception) ??
        std::memcpy(buffer, msg_buffer, msg_size);
        //  > check errors
        size = msg_size;
    }

    Message::Message(const Message &message)
    {
        buffer = new char[message.size];
        std::memcpy(buffer, message.buffer, message.size);
        size = message.size;
    }

/*
    Message& Message::operator=(const Message& message) // copy assignment
    {
        if (this != &message) { // self-assignment check expected
            if (message.size != size) {         // storage cannot be reused
                delete[] buffer;
                size = 0;
                buffer = nullptr;               // preserve invariants in case next line throws
                buffer = new char[message.size];
                size = message.size;
            } 
            std::copy(message.buffer, message.buffer + message.size, buffer);
        }
        return *this;
    }
*/

    Message::~Message()
    {
        delete [] buffer;
        size = 0;
    }
}

namespace threads_layer
{
    // TODO?, TODOCHECK thrwoing exceptions in ctor/dtor ?
    BlockingMessageQueue::BlockingMessageQueue()
    {
        int sync_op_result;
        if( (sync_op_result = pthread_mutex_init(&access_mutex_, (pthread_mutexattr_t *) NULL)) != 0 )
        // or: PTHREAD_MUTEX_INITIALIZER
        {
            std::cerr << "Error initializing mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }

        if( (sync_op_result = pthread_cond_init(&empty_cond_, (pthread_condattr_t *) NULL)) != 0 )
        // or: PTHREAD_COND_INITIALIZER
        {
            std::cerr << "Error initializing condition variable\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }
    }

    // TODOCHECK, TODOWARY, TODO? It shall be safe to destroy an initialized mutex that is unlocked. Attempting to destroy a locked mutex results in undefined behavior.
    // It shall be safe to destroy an initialized condition variable upon which no threads are currently blocked. Attempting to destroy a condition variable upon which other threads are currently blocked results in undefined behavior.
    BlockingMessageQueue::~BlockingMessageQueue()
    {
        int sync_op_result;
        if( (sync_op_result = pthread_mutex_destroy(&access_mutex_)) != 0 )
        {
            std::cerr << "Error destroying mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }

        if( (sync_op_result = pthread_cond_destroy(&empty_cond_)) != 0 )
        {
            std::cerr << "Error destroying condition variable\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }
    }

    void BlockingMessageQueue::enqueue(const messages::Message &message)
    {
        int sync_op_result;    // TODO? does not have to be atomic
        if( (sync_op_result = pthread_mutex_lock(&access_mutex_)) != 0 )
        {
            std::cerr << "Error locking mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }

        // chceck available space (pthread_cond)
        // TODOCHECK can std::queue be full? Some exception when insufficient memory?
        // enqueue
        // if size == 1 -> signal cond

        queue_.push( std::move(message) );  // TODO does it require additional move assignment operator overload?
        //queue_.push(message);

        if( queue_.size() == 1 ) // size_type
        {
            // TODO? or broadcast?
            if( (sync_op_result = pthread_cond_signal(&empty_cond_)) != 0 )
            {
                std::cerr << "Error signaling condition\n"
                             "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
            }
        }

        if( (sync_op_result = pthread_mutex_unlock(&access_mutex_)) != 0 )
        {
            std::cerr << "Error unlocking mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }
    }

    messages::Message BlockingMessageQueue::dequeue()
    {
        int sync_op_result;
        if( (sync_op_result = pthread_mutex_lock(&access_mutex_)) != 0 )
        {
            std::cerr << "Error locking mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }
 
        // chceck available messages (pthread_cond)
        // dequeue
        // Message <- move/copy-constructible

        // IMPORTANT IMPLEMENTATION NOTE: When using condition variables there is always a Boolean predicate involving shared variables associated with each condition wait that is true if the thread should proceed. Spurious wakeups from the pthread_cond_timedwait() or pthread_cond_wait() functions may occur. Since the return from pthread_cond_timedwait() or pthread_cond_wait() does not imply anything about the value of this predicate, the predicate should be re-evaluated upon such return.
        while( queue_.empty() ) // thus while, not if
        {
            if( (sync_op_result = pthread_cond_wait(&empty_cond_, &access_mutex_)) != 0 )
            {
                std::cerr << "Error waiting on condition variable\n"
                             "Error: " << std::strerror(sync_op_result) << std::endl;

                throw std::system_error(sync_op_result, std::generic_category());
            }
        }

        messages::Message message = queue_.front();
        queue_.pop();

        if( (sync_op_result = pthread_mutex_unlock(&access_mutex_)) != 0 )
        {
            std::cerr << "Error unlocking mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }

        return message; // copy TODO make it better
    }


    // Mutex must be released before attempting to destroy it.
    // (Attempting to destroy a locked mutex results in undefined behavior.)
    void BlockingMessageQueue::release_access_mutex_on_thread_cancel_cleanup() const
    {
        int sync_op_result;
        if( (sync_op_result = pthread_mutex_unlock(&access_mutex_)) != 0 )
        {
            std::cerr << "Error unlocking mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }
    }
}
