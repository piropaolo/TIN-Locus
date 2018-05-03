#include "ClientManager.h"
#include "SessionManager.h"

#include <system_error>

#include <utility>

extern "C" {
  #include <unistd.h> // standard symbolic constants and types (e.g. STDOUT_FILENO)
}

#include <list>
// TODO temporary; temporarily here
extern clients::SessionManager session_manager; // declaration; defined in server.cpp 


#include <iostream>
#include <cassert>

namespace clients
{
    namespace internals
    {
        /*
        https://isocpp.org/wiki/faq/mixing-c-and-cpp
        extern "C" double call_C_f(struct C* p, int i) // wrapper function
        {
            return p->f(i);
        }
        */

        /*void close_socket_in_thread(int socket_fd)
        {
            std::cout << "--Exiting thread for socket_fd: " << socket_fd << "--" << std::endl;

            // TODO general macros for checking error conditions + throwing exceptions?
            if( close(socket_fd) != 0 )
            {
                std::cerr << "Error closing socket. Socket_fd: " << socket_fd << "\n"
                            "Error: " << std::strerror(errno) << std::endl;
                pthread_exit((void *) NULL);

                // TODO or:
                //throw std::system_error(errno, std::generic_category());
            }
        }*/

        // TODO?? will calling this from C function work?
        /*extern "C" struct ClassMethodWrapper
        {
            // ::clients::ClientManager *object_pointer;
            // void (::clients::ClientManager::* method_pointer)(void); /*= &::clients::ClientManager::sender_thread_routine; // pointer to class member function
        };*/


        // TODO? pthread_cleanup handlers for closing socket
        // NOTE: cannot be a class method because of C-language linkage
        // C-language linkage wrapper for a method class ? 
        // can use C++ code
        extern "C" void * receiver_thread_routine_wrapper(void *comm_manager)
        {
            ::clients::ClientManager *commManager = (::clients::ClientManager *) comm_manager;

            commManager->receiver_thread_routine();

            return (void *) NULL;
        }

        // TODO wrapper for class method? -- maybe method of class BlockingMessageQueue (unlocking its mutex)
        extern "C" /*static ?*/ void sender_thread_cleanup_handler(void *comm_manager)
        {
            // arg not used (yet)
            const char cleanup_msg[] = "--SenderThread cancelled, cleanup done--\n";

            // returns number of bytes written or -1 and sets errno
            write(STDOUT_FILENO, (const void *) cleanup_msg, sizeof cleanup_msg - 1); // without the terminating '\0'

            // TODO? is it neccessary to use signal-safe functions here??

            ((clients::ClientManager *) comm_manager)->message_queue_.release_access_mutex_on_thread_cancel_cleanup();
        }

        /*extern "C"*/ void * sender_thread_routine_wrapper(void *comm_manager)
        {
            // TODO? RAII class SenderThreadCleanupGuard (ctor: push, dtor: pop) ?? would it work? (rather not)
            pthread_cleanup_push(&sender_thread_cleanup_handler, comm_manager);

            ClientManager *commManager = (clients::ClientManager *) comm_manager;

            commManager->sender_thread_routine();

            /*
            TEST:
            while(true)
                pthread_testcancel();
            */

            pthread_cleanup_pop(0); // do not execute the cleanup handler

            return (void *) NULL;
        }  
    }

    void ClientManager::sender_thread_routine()
    {
        //pthread_cleanup_push(sender_thread_cleanup_handler, (void *) NULL);

        char weclome_message[] = "\n"
                                "This is a welcome message.\n"
                                "Hello. You've just connected to the TestTcp server.\n"
                                "\n";
                                //"Your client id is " 
        try
        {
            comm_endpoint_ptr_->sendNBytes(weclome_message, sizeof weclome_message);   

            while( true )
            {
                // block untill there is a message to send to the client
                // TODO? add operator= for Message ?
                messages::Message message = message_queue_.dequeue();

                //DBG std::cout << "cpy in ClientManager.sender_thread_routine?" << std::endl;
                comm_endpoint_ptr_->sendNBytes(message.buffer, message.size);
                // TODO how to exit? When connection closed... -- signal? pthread_cancel? special_message (=> needs message parsing :/)?
                // receiver_thread knows when client disconnected -> signal/pthread_cancel to sender_thread
                // what when the server wants to disconnect from the client (abort connection) (but not exit whole server) -> which thread should send signal/pthread_cancel to sender and receiver?
                // ? additional thread for executing orders from the server? -- per client or 1 for all clients?
            }
        }
        catch( const std::system_error &ex )
        {
            std::cerr << "Error sending message.\n"
                         "Error: " << ex.code().message() << std::endl;
            // TODO test error codes:
            // (EAGAIN or EWOULDBLOCK), ECONNRESET, (EINTR), EPIPE, EACCES, EIO, ENETDOWN, ENETUNREACH
            
            //commSocket.close();

            std::cout << "--SenderThread exited on exception--" << std::endl;
            return;
        }

        // TODO? shutdown(sock_fd, SHUT_WR)

        // TODO? can be canceled (pthread_cond_wait in message_queue_.dequeue() is a cancellation point). So does this thread need a cancellation clean-up handler in which it will unlock the message_queue_ access_mutex?
        // TODO? recognize if the sender thread was cancelled during wait on monitor (message_queue_.dequeue()) or during send() (multiple possible cancellation points). Unolock the message_queue_'s access_mutex (so that it can be safely destroyed as unlocked) only when cancelled inside this critical section! (Attempting to unlock the mutex if it was not locked by the calling thread results in undefined behavior. Attempting to unlock the mutex if it is not locked results in undefined behavior.)
        // NOTE:
        // Cancellation and Condition Wait:
        // A condition wait, whether timed or not, is a cancellation point. That is, the functions pthread_cond_wait() or pthread_cond_timedwait() are points where a pending (or concurrent) cancellation request is noticed. The reason for this is that an indefinite wait is possible at these points-whatever event is being waited for, even if the program is totally correct, might never occur; for example, some input data being awaited might never be sent. By making condition wait a cancellation point, the thread can be canceled and perform its cancellation cleanup handler even though it may be stuck in some indefinite wait.
        // A side effect of acting on a cancellation request while a thread is blocked on a condition variable is to re-acquire the mutex before calling any of the cancellation cleanup handlers. This is done in order to ensure that the cancellation cleanup handler is executed in the same state as the critical code that lies both before and after the call to the condition wait function. This rule is also required when interfacing to POSIX threads from languages, such as Ada or C++, which may choose to map cancellation onto a language exception; this rule ensures that each exception handler guarding a critical section can always safely depend upon the fact that the associated mutex has already been locked regardless of exactly where within the critical section the exception was raised. Without this rule, there would not be a uniform rule that exception handlers could follow regarding the lock, and so coding would become very cumbersome.
        // Therefore, since some statement has to be made regarding the state of the lock when a cancellation is delivered during a wait, a definition has been chosen that makes application coding most convenient and error free.
        // When acting on a cancellation request while a thread is blocked on a condition variable, the implementation is required to ensure that the thread does not consume any condition signals directed at that condition variable if there are any other threads waiting on that condition variable. This rule is specified in order to avoid deadlock conditions that could occur if these two independent requests (one acting on a thread and the other acting on the condition variable) were not processed independently. 

        // never reached
        std::cout << "--SenderThread exited--" << std::endl;
    }
    
    void ClientManager::receiver_thread_routine() /*const*/
    {
        char message_buffer[MSG_BUF_SIZE];
        int bytes_received = 0;

        try
        {
            do
            {
                bytes_received = comm_endpoint_ptr_->receive(message_buffer, MSG_BUF_SIZE);

                // TODO DBG cout client id
                if( bytes_received == 0 ) // TODO temporary
                    break;

                std::cout << bytes_received << " bytes received. Message:\n" <<
                            "> ";
                std::cout.write(message_buffer, bytes_received);
                std::cout << std::endl;

                // echo received message to the client
                // TODO message should be allocated or bad things will hapen :(
                this->sendToPeersInSession( messages::Message(message_buffer, bytes_received) );
                //or: message_queue_.enqueue(message);
            }
            while( true ); // temporary; need exception for connection shutdown -> then: while( true ) {..}
            // TODO without connection-close exception server will send 0-byte-length message to client when he disconnects
        }
        catch( const std::system_error &ex )
        {
            std::cerr << "Error in communication loop.\n"   // TODO recognize the error - maybe connection loss?
                         "Error: " << ex.code().message() << std::endl;
            // TODO test error codes:
            // (EAGAIN or EWOULDBLOCK), ECONNRESET, (EINTR), EPIPE, EACCES, EIO, ENETDOWN, ENETUNREACH
            
            cancel_sender_thread();   // TODO?

            // TODO eliminate code repetition
            void *thread_exit_status;
            int thread_join_result = 0;
            if( (thread_join_result = pthread_join(sender_thread_, &thread_exit_status)) != 0 )
            {
                // DBG cerr
                std::cerr << "Error joining with SenderThread\n"
                             "Error: " << std::strerror(thread_join_result) << std::endl;
                throw std::system_error(thread_join_result, std::generic_category());
            }
            assert(thread_exit_status == PTHREAD_CANCELED);

            //commSocket.close();
            comm_endpoint_ptr_->close();
            removeClient();

            std::cout << "--ReceiverThread exited on exception--" << std::endl;
            return;
        }

        // (FINALLY block):
        // TODO? dtor of some context object ?

        cancel_sender_thread();
        // TODO? pthread_join with SenderThread ?
        // After a canceled thread has terminated, a join with that thread using pthread_join(3) obtains PTHREAD_CANCELED as the thread's exit status. (Joining with a thread is the only way to know that cancellation has completed.) 

        // Synchronize (wait for SenderThread to actually end (and perform neccessary clean-up))
        // pthread_join (joinable SenderThread)
        // usleep/sleep/std::this_thread::sleep_for
        // pthread_yield (nonstandard) / sched_yield
        // YIELD:
        // Strategic calls to sched_yield() can improve performance by giving other threads or processes a chance to run when (heavily) contended resources (e.g., mutexes) have been released by the caller. Avoid calling sched_yield() unnecessarily or inappropriately (e.g., when resources needed by other schedulable threads are still held by the caller), since doing so will result in unnecessary context switches, which will degrade system performance.
        std::cout << "DBG: ReceiverThread joining with SenderThread" << std::endl;
        void *thread_exit_status;
        int thread_join_result = 0;
        if( (thread_join_result = pthread_join(sender_thread_, &thread_exit_status)) != 0 )
        {
            // DBG cerr
            std::cerr << "Error joining with SenderThread\n"
                         "Error: " << std::strerror(thread_join_result) << std::endl;
            throw std::system_error(thread_join_result, std::generic_category());
        }
        assert(thread_exit_status == PTHREAD_CANCELED);


        //close_socket_in_thread(sock_fd);
        // TODO inside try-catch
        //commSocket.close();
        comm_endpoint_ptr_->close();
        removeClient();

        std::cout << "--ReceiverThread exited--" << std::endl;
    }

    void ClientManager::cancel_sender_thread()
    {
        int thread_cancellation_result = 0;
        if( (thread_cancellation_result = pthread_cancel(sender_thread_)) != 0 )
        {
            // DBG cerr
            std::cerr << "Error sending cancellation request to SenderThread\n"
                         "Error: " << std::strerror(thread_cancellation_result) << std::endl;
            throw std::system_error(thread_cancellation_result, std::generic_category());
        }

        // DBG
        std::cout << "-Cancellation request sent to SenderThread-" << std::endl;
    }


    /*ClientManager::ClientManager(const comm_layer::CommSocketEndpoint &comm_endpoint) : comm_endpoint_(comm_endpoint)*/
    ClientManager::ClientManager(/*const*/ std::unique_ptr< comm_layer::CommSocketEndpoint > comm_endpoint_ptr)    // && ?
    {
        //comm_endpoint_ptr_ = std::move(comm_endpoint_ptr);
        comm_endpoint_ptr_.swap(comm_endpoint_ptr);
        // std::swap ?
        client_id_ = ++client_id_counter_;
        std::cout << "--ClientManager.ctor--" << std::endl;
    }

    ClientManager::~ClientManager() 
    {
        std::cout << "--ClientManager.dtor--" << std::endl;
    } // TODO? pthread_join here ?? (for joinable threads) -- would be called when ClientManager object removed from ClientManagersList (?)

    void ClientManager::startFullDuplexComm()
    {
        // TODO create thread attributes object only once (for every ClientManager); destroy it on server exit
        pthread_t thread_id;    // thread handler
        pthread_attr_t thread_attributes;
        int thread_op_result;

        if( (thread_op_result = pthread_attr_init(&thread_attributes)) != 0 )
        {
            std::cerr << "Error initializing thread attributes object\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
        }
        // create thread in detached state
        if( (thread_op_result = pthread_attr_setdetachstate(&thread_attributes, PTHREAD_CREATE_DETACHED)) != 0 )
        {
            std::cerr << "Error setting the detach state attribute in thread attributes object\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
        }

        // TODO thread_routine's arg <- struct/class
        // create detached receiver thread
        // TODO? allocate CommEndpoint object on heap ?
        if( (thread_op_result = pthread_create(&thread_id, &thread_attributes, &internals::receiver_thread_routine_wrapper, (void *) this)) != 0 )
        {
            std::cerr << "Error creating new thread\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
            // TODO? display message and continue; ? (at accept loop)
        }
        receiver_thread_ = thread_id;
        std::cout << "--ClientManager: started receiver thread--" << std::endl;
        std::cout << "DBG: thread id: " << receiver_thread_ << std::endl;

        
        // create joinable sender thread
        if( (thread_op_result = pthread_create(&thread_id, (pthread_attr_t *) NULL, &internals::sender_thread_routine_wrapper, (void *) this)) != 0 )
        {
            std::cerr << "Error creating new thread\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
        }
        sender_thread_ = thread_id;
        std::cout << "--ClientManager: started sender thread--" << std::endl;
        std::cout << "DBG: thread id: " << sender_thread_ << std::endl;     


        if( (thread_op_result = pthread_attr_destroy(&thread_attributes)) != 0 )
        {
            std::cerr << "Error destroying thread attributes object\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
        }
    }

    void ClientManager::send(const messages::Message &message)
    {
        //DBG std::cout << "cpy in ClientManager.send?" << std::endl;
        message_queue_.enqueue(message);
    }

    void ClientManager::sendToPeersInSession(const messages::Message &message) const
    {
        session_manager.sendToAllClientsOtherThan(*this, message);
    }

    void ClientManager::sendToAllInSession(const messages::Message &message) const
    {
        session_manager.sendToAllClients(message);
    }

    void ClientManager::removeClient() const
    {
        //clients_list.remove(*this);
        // uses operator==
        int client_id = this->client_id_;

        /*clients_list.remove_if( 
            [this](const std::unique_ptr< clients::ClientManager > &element_ptr)
            {
                return *this == *element_ptr;
            } 
        );*/

        session_manager.removeClient(*this);

        std::cout << "DBG: Client id: " << client_id << " removed from Session." << std::endl;
    }

    bool ClientManager::operator==(const ClientManager &other_client) const
    {
        std::cout << "DBG: Client operator==" << std::endl;
        return (client_id_ == other_client.client_id_);
    }

    bool ClientManager::operator!=(const ClientManager &other_client) const
    {
        return !(*this == other_client);
    }

    int ClientManager::client_id_counter_ = 0;
}

// TODOTHREAD_CLEANUP
// When a thread terminates by calling pthread_exit(3), all clean-up handlers are executed. (Clean-up handlers are not called if the thread terminates by performing a return from the thread start function.)
// IMPORTANT IMPLEMENTATION NOTES:
// POSIX.1 permits pthread_cleanup_push() and pthread_cleanup_pop() to be implemented as macros that expand to text containing '{' and '}', respectively. For this reason, the caller must ensure that calls to these functions are paired within the same function, and at the same lexical nesting level. (In other words, a clean-up handler is only established during the execution of a specified section of code.)
// On Linux, the pthread_cleanup_push() and pthread_cleanup_pop() functions are implemented as macros that expand to text containing '{' and '}', respectively. This means that variables declared within the scope of paired calls to these functions will only be visible within that scope.
// POSIX.1 says that the effect of using return, break, continue, or goto to prematurely leave a block bracketed pthread_cleanup_push() and pthread_cleanup_pop() is undefined. Portable applications should avoid doing this. 
// LONGJMP:
// Calling longjmp(3) (siglongjmp(3)) produces undefined results if any call has been made to pthread_cleanup_push() or pthread_cleanup_pop() without the matching call of the pair since the jump buffer was filled by setjmp(3) (sigsetjmp(3)). Likewise, calling longjmp(3) (siglongjmp(3)) from inside a clean-up handler produces undefined results unless the jump buffer was also filled by setjmp(3) (sigsetjmp(3)) inside the handler. 