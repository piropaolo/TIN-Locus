#include "ClientManager.h"
#include "SessionManager.h"

#include <system_error>

#include <utility>

#include <sstream>
#include "log/Logger.h"

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
        // NOTE: cannot be a class method because of C-language linkage
        // C-language linkage wrapper for a method class
        // can use C++ code
        extern "C" void * receiver_thread_routine_wrapper(void *comm_manager)
        {
            ::clients::ClientManager *commManager = (::clients::ClientManager *) comm_manager;

            commManager->receiver_thread_routine();

            return (void *) NULL;
        }

        // TODO wrapper for class method?
        extern "C" /*static ?*/ void sender_thread_cleanup_handler(void *comm_manager)
        {
            // TODO? is it neccessary to use signal-safe functions here??
            ((clients::ClientManager *) comm_manager)->message_queue_.release_access_mutex_on_thread_cancel_cleanup();
        }

        extern "C" void * sender_thread_routine_wrapper(void *comm_manager)
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

            pthread_cleanup_pop(0); // do not execute the cleanup handler, only pop it from the thread's handlers stack

            return (void *) NULL;
        }  
    }

    void ClientManager::sender_thread_routine()
    {
        try
        {   
            while( true )
            {
                // block untill there is a message to send to the client
                // TODO? add operator= for Message ?
                messages::Message message = message_queue_.dequeue();

                messenger_ptr_->send(message.buffer, message.size);

                // how to exit? When connection closed... -- signal? pthread_cancel? special_message (=> needs message parsing :/)?
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

            return;
        }

        // TODO? shutdown(sock_fd, SHUT_WR)

        // never reached
    }
    
    void ClientManager::receiver_thread_routine() /*const*/
    {
        std::ostringstream log_stream;
        char message_buffer[MSG_BUF_SIZE];
        int bytes_received = 0;

        try
        {
            do
            {
                bytes_received =  messenger_ptr_->recvAny(message_buffer, MSG_BUF_SIZE);

                // TODO DBG cout client id
                if( bytes_received == 0 ) // TODO temporary
                    break;
         
                log_stream << bytes_received << " bytes received. Message:\n" <<
                            "> ";
                log_stream.write(message_buffer, bytes_received);
                log::Logger::getInstance().logDebug( log_stream.str() );
                log_stream.str("");

                this->sendToAllInSession( messages::Message(message_buffer, bytes_received) );
            }
            while( true ); // temporary; need exception for connection shutdown -> then: while( true ) {..}
            // TODO without connection-close exception server will send 0-byte-length message to client when he disconnects
            // for now: temoprary solution above
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

            messenger_ptr_->close();
            removeClient();

            return;
        }

        // (FINALLY block):
        // TODO? dtor of some context object ?

        cancel_sender_thread();

        // Synchronize (wait for SenderThread to actually end (and perform neccessary clean-up))
        // pthread_join (joinable SenderThread)
        // usleep/sleep/std::this_thread::sleep_for
        // pthread_yield (nonstandard) / sched_yield
        // YIELD:
        // Strategic calls to sched_yield() can improve performance by giving other threads or processes a chance to run when (heavily) contended resources (e.g., mutexes) have been released by the caller. Avoid calling sched_yield() unnecessarily or inappropriately (e.g., when resources needed by other schedulable threads are still held by the caller), since doing so will result in unnecessary context switches, which will degrade system performance.
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


        // TODO inside try-catch
        messenger_ptr_->close();

        removeClient();
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
    }


    /*ClientManager::ClientManager(const messenger::Messenger &comm_endpoint) : comm_endpoint_(comm_endpoint)*/
    ClientManager::ClientManager(std::unique_ptr< messenger::Messenger > messenger_ptr)    // &&-ref ?
    {
        //messenger_ptr_ = std::move(comm_endpoint_ptr);
        messenger_ptr_.swap(messenger_ptr);
        // std::swap ?
        client_id_ = ++client_id_counter_;
    }

    ClientManager::~ClientManager() 
    {} // TODO? pthread_join here ?? (for joinable threads) -- would be called when ClientManager object removed from ClientManagersList (?)

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

        // create detached receiver thread
        if( (thread_op_result = pthread_create(&thread_id, &thread_attributes, &internals::receiver_thread_routine_wrapper, (void *) this)) != 0 )
        {
            std::cerr << "Error creating new thread\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
            // TODO? display message and continue; ? (at accept loop)
        }
        receiver_thread_ = thread_id;

        
        // create joinable sender thread
        if( (thread_op_result = pthread_create(&thread_id, (pthread_attr_t *) NULL, &internals::sender_thread_routine_wrapper, (void *) this)) != 0 )
        {
            std::cerr << "Error creating new thread\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
            // TODO? display message and continue; ? (at accept loop)
        }
        sender_thread_ = thread_id;   


        if( (thread_op_result = pthread_attr_destroy(&thread_attributes)) != 0 )
        {
            std::cerr << "Error destroying thread attributes object\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
        }
    }

    void ClientManager::send(const messages::Message &message)
    {
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
        session_manager.removeClient(*this);
    }

    bool ClientManager::operator==(const ClientManager &other_client) const
    {
        return (client_id_ == other_client.client_id_);
    }

    bool ClientManager::operator!=(const ClientManager &other_client) const
    {
        return !(*this == other_client);
    }

    int ClientManager::client_id_counter_ = 0;
}

// NOTES:
// THREAD_CLEANUP
// When a thread terminates by calling pthread_exit(3), all clean-up handlers are executed. (Clean-up handlers are not called if the thread terminates by performing a return from the thread start function.)
// IMPORTANT IMPLEMENTATION NOTES:
// POSIX.1 permits pthread_cleanup_push() and pthread_cleanup_pop() to be implemented as macros that expand to text containing '{' and '}', respectively. For this reason, the caller must ensure that calls to these functions are paired within the same function, and at the same lexical nesting level. (In other words, a clean-up handler is only established during the execution of a specified section of code.)
// On Linux, the pthread_cleanup_push() and pthread_cleanup_pop() functions are implemented as macros that expand to text containing '{' and '}', respectively. This means that variables declared within the scope of paired calls to these functions will only be visible within that scope.
// POSIX.1 says that the effect of using return, break, continue, or goto to prematurely leave a block bracketed pthread_cleanup_push() and pthread_cleanup_pop() is undefined. Portable applications should avoid doing this.