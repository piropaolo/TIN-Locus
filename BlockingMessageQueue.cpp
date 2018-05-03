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
        // TODO?
        buffer = new char[msg_size];

        //DBG std::cout << "Sizeof buffer in ctor: " << sizeof buffer << std::endl;

        //  > check errors (std::bad_alloc exception) ??
        std::memcpy(buffer, msg_buffer, msg_size);
        //  > check errors
        size = msg_size;
    }

    Message::Message(const Message &message)
    {
        // checked in ctor
        //if( msg_buffer == nullptr )
        //    throw std::invalid_argument("Invalid message buffer address: nullptr.");
        
        //DBG std::cout << "Message copy ctor called" << std::endl;

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

        //DBG std::cout << "cpy on queue_.push() in BlockingMessageQueue.enqueue?" << std::endl;
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

        //DBG std::cout << "cpy on queue_.front() in BlockingMessageQueue.dequeue?" << std::endl;
        messages::Message message = queue_.front();
        queue_.pop();

        if( (sync_op_result = pthread_mutex_unlock(&access_mutex_)) != 0 )
        {
            std::cerr << "Error unlocking mutex\n"
                         "Error: " << std::strerror(sync_op_result) << std::endl;

            throw std::system_error(sync_op_result, std::generic_category());
        }

        //DBG std::cout << "cpy on exit from BlockingMessageQueue.dequeue?" << std::endl;
        return message; // copy TODO make it better
    }
}

// TODOMUTEX
// It shall be safe to destroy an initialized mutex that is unlocked. Attempting to destroy a locked mutex results in undefined behavior. 
// MUTEX INIT:
// In cases where default mutex attributes are appropriate, the macro PTHREAD_MUTEX_INITIALIZER can be used to initialize mutexes that are statically allocated. The effect shall be equivalent to dynamic initialization by a call to pthread_mutex_init() with parameter attr specified as NULL, except that no error checks are performed.
// MUTEX DESTROY:
// A mutex can be destroyed immediately after it is unlocked.
// RECURSIVE MUTEX AND COND_WAIT:
// It is advised that an application should not use a PTHREAD_MUTEX_RECURSIVE mutex with condition variables because the implicit unlock performed for a pthread_cond_timedwait() or pthread_cond_wait() may not actually release the mutex (if it had been locked multiple times). If this happens, no other thread can satisfy the condition of the predicate.

// TODOCOND
// condition variable wait, timedwait:
// These functions atomically release mutex and cause the calling thread to block on the condition variable cond; atomically here means "atomically with respect to access by another thread to the mutex and then the condition variable". That is, if another thread is able to acquire the mutex after the about-to-block thread has released it, then a subsequent call to pthread_cond_broadcast() or pthread_cond_signal() in that thread shall behave as if it were issued after the about-to-block thread has blocked.
// Upon successful return, the mutex shall have been locked and shall be owned by the calling thread.
// The effect of using more than one mutex for concurrent pthread_cond_timedwait() or pthread_cond_wait() operations on the same condition variable is undefined; that is, a condition variable becomes bound to a unique mutex when a thread waits on the condition variable, and this (dynamic) binding shall end when the wait returns.
// If more than one thread is blocked on a condition variable, the scheduling policy shall determine the order in which threads are unblocked. When each thread unblocked as a result of a pthread_cond_broadcast() or pthread_cond_signal() returns from its call to pthread_cond_wait() or pthread_cond_timedwait(), the thread shall own the mutex with which it called pthread_cond_wait() or pthread_cond_timedwait(). The thread(s) that are unblocked shall contend for the mutex according to the scheduling policy (if applicable), and as if each had called pthread_mutex_lock().
// COND INIT:
// In cases where default condition variable attributes are appropriate, the macro PTHREAD_COND_INITIALIZER can be used to initialize condition variables that are statically allocated. The effect shall be equivalent to dynamic initialization by a call to pthread_cond_init() with parameter attr specified as NULL, except that no error checks are performed.
// COND DESTROY:
// A condition variable can be destroyed immediately after all the threads that are blocked on it are awakened.
// (e.g. after a pthread_cond_broadcast on this condition variable)

// TODOCOND, TODOTHREAD_CANCEL
// A condition wait (whether timed or not) is a cancellation point. When the cancelability enable state of a thread is set to PTHREAD_CANCEL_DEFERRED, a side effect of acting upon a cancellation request while in a condition wait is that the mutex is (in effect) re-acquired before calling the first cancellation cleanup handler. The effect is as if the thread were unblocked, allowed to execute up to the point of returning from the call to pthread_cond_timedwait() or pthread_cond_wait(), but at that point notices the cancellation request and instead of returning to the caller of pthread_cond_timedwait() or pthread_cond_wait(), starts the thread cancellation activities, which includes calling cancellation cleanup handlers. 
// A thread that has been unblocked because it has been canceled while blocked in a call to pthread_cond_timedwait() or pthread_cond_wait() shall not consume any condition signal that may be directed concurrently at the condition variable if there are other threads blocked on the condition variable.

// TODOCOND, TODOSIGNAL
// If a signal is delivered to a thread waiting for a condition variable, upon return from the signal handler the thread resumes waiting for the condition variable as if it was not interrupted, or it shall return zero due to spurious wakeup.
// It is not safe to use the pthread_cond_signal() function in a signal handler that is invoked asynchronously. Even if it were safe, there would still be a race between the test of the Boolean pthread_cond_wait() that could not be efficiently eliminated.
// Mutexes and condition variables are thus not suitable for releasing a waiting thread by signaling from code running in a signal handler. 

// TODOCOND_WAIT
// It is important to note that when pthread_cond_wait() and pthread_cond_timedwait() return without error, the associated predicate may still be false. Similarly, when pthread_cond_timedwait() returns with the timeout error, the associated predicate may be true due to an unavoidable race between the expiration of the timeout and the predicate state change.
// In general, whenever a condition wait returns, the thread has to re-evaluate the predicate associated with the condition wait to determine whether it can safely proceed, should wait again, or should declare a timeout. A return from the wait does not imply that the associated predicate is either true or false.
// It is thus recommended that a condition wait be enclosed in the equivalent of a "while loop" that checks the predicate.

// TODOCOND_SIGNAL__BROADCAST
// The pthread_cond_broadcast() or pthread_cond_signal() functions may be called by a thread whether or not it currently owns the mutex that threads calling pthread_cond_wait() or pthread_cond_timedwait() have associated with the condition variable during their waits; however, if predictable scheduling behavior is required, then that mutex shall be locked by the thread calling pthread_cond_broadcast() or pthread_cond_signal().
// The pthread_cond_broadcast() and pthread_cond_signal() functions shall have no effect if there are no threads currently blocked on cond.
// TODOCOND_SIGNAL
// On a multi-processor, it may be impossible for an implementation of pthread_cond_signal() to avoid the unblocking of more than one thread blocked on a condition variable.
// The effect is that more than one thread can return from its call to pthread_cond_wait() or pthread_cond_timedwait() as a result of one call to pthread_cond_signal(). This effect is called "spurious wakeup". Note that the situation is self-correcting in that the number of threads that are so awakened is finite.

// TODOCOND, TODOMUTEX, SCHEDULING_POLICY
// Synchronization primitives that attempt to interfere with scheduling policy by specifying an ordering rule are considered undesirable. Threads waiting on mutexes and condition variables are selected to proceed in an order dependent upon the scheduling policy rather than in some fixed order (for example, FIFO or priority). Thus, the scheduling policy determines which thread(s) are awakened and allowed to proceed.