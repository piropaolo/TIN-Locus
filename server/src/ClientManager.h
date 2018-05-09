#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

//#include "SocketEndpoint.h"
#include "messenger/Messenger.h"
#include "BlockingMessageQueue.h"

extern "C" {
  #include <pthread.h>  // POSIX threads
  #include <sys/types.h>
}

#include <memory>

namespace clients
{
    namespace internals
    {
        extern "C" void * receiver_thread_routine_wrapper(void *comm_manager);

        extern "C" void * sender_thread_routine_wrapper(void *comm_endpoint);

        extern "C" void sender_thread_cleanup_handler(void *comm_manager);
    }

    // CommManager ?
    class ClientManager
    {
        private:
          // TODO temporary solution
          static int client_id_counter_;
          // TODO int?
          int client_id_;
          /*comm_layer::CommSocketEndpoint comm_endpoint_;*/
          std::unique_ptr< messenger::Messenger > messenger_ptr_;

          // TODO? lazy creation (in startFullDuplexComm, not in ctor) ?
          threads_layer::BlockingMessageQueue message_queue_;

          // TODO?
          //SessionManager &session_manager_;
          //or: std::list<SessionManager &> sessions_;

          pthread_t sender_thread_;
          pthread_t receiver_thread_;

          const std::size_t MSG_BUF_SIZE = 100;

        private:
          void sender_thread_routine();
          void receiver_thread_routine() /*const*/;

          void cancel_sender_thread(); // TODO rename: stop_sender_thread

          friend void * internals::receiver_thread_routine_wrapper(void *comm_manager);
          friend void * internals::sender_thread_routine_wrapper(void *comm_manager);
          friend void internals::sender_thread_cleanup_handler(void *comm_manager);

        public:
          // TODO add base class CommEndpoint
          /*ClientManager(const comm_layer::CommSocketEndpoint &comm_endpoint);*/
          ClientManager(std::unique_ptr< messenger::Messenger > messenger_ptr);
          ~ClientManager();

          // start full-duplex communication with the client (async)
          void startFullDuplexComm();
          // send message to the client (async)
          void send(const messages::Message &message);

          // send message to all the peers in the current session
          void sendToPeersInSession(const messages::Message &message) const;

          void sendToAllInSession(const messages::Message &message) const;

          // remove Client from the (global ? singleton ?) SessionManager
          void removeClient() const;
          bool operator==(const ClientManager &other_client) const;
          bool operator!=(const ClientManager &other_client) const;

          //void close();   // close connection with the client (initiated by the server)

          // TODO remove client from SessionManager on error!! (e.g. thread exited because of error, and there is no connection with the client host)

          // TODO Strategy Pattern (strategy of sending - to peers / to all clients / echo)
    };
}

#endif /*CLIENT_MANAGER_H*/