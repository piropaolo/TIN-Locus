#include "ClientManager.h"

#include <system_error>

#include <iostream>
#include <utility>

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

        /*extern "C"*/ void * sender_thread_routine_wrapper(void *comm_manager)
        {
            ClientManager *commManager = (clients::ClientManager *) comm_manager;

            commManager->sender_thread_routine();

            return (void *) NULL;
        }
    }

    void ClientManager::sender_thread_routine()
    {
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
                this->send( messages::Message(message_buffer, bytes_received) );
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

            //commSocket.close();
            comm_endpoint_ptr_->close();

            std::cout << "--ReceiverThread exited on exception--" << std::endl;
            return;
        }

        cancel_sender_thread();
        // TODO? pthread_join with SenderThread ?
        // After a canceled thread has terminated, a join with that thread using pthread_join(3) obtains PTHREAD_CANCELED as the thread's exit status. (Joining with a thread is the only way to know that cancellation has completed.) 

        //close_socket_in_thread(sock_fd);
        // TODO inside try-catch
        //commSocket.close();
        comm_endpoint_ptr_->close();

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
        std::cout << "--ClientManager.ctor--" << std::endl;
    }

    ClientManager::~ClientManager() 
    {
        std::cout << "--ClientManager.dtor--" << std::endl;
    } // TODO? pthread_join here ?? (for joinable threads) -- would be called when ClientManager object removed from ClientManagersList (?)

    void ClientManager::startFullDuplexComm()
    {
        // TODO create thread attributes object only once (for every ClientManager); destry it on server exit
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
        // create receiver thread
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

        
        // create sender thread
        if( (thread_op_result = pthread_create(&thread_id, &thread_attributes, &internals::sender_thread_routine_wrapper, (void *) this)) != 0 )
        {
            std::cerr << "Error creating new thread\n"
                         "Error: " << std::strerror(thread_op_result) << std::endl;
            throw std::system_error(thread_op_result, std::generic_category());
        }
        sender_thread_ = thread_id;
        std::cout << "--ClientManager: started sender thread--" << std::endl;


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
}