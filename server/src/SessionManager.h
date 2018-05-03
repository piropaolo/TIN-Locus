#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "ClientManager.h"

#include <list>
#include <memory> // std::make_unique (needs C++14), std::unique_ptr

#include <cstring>  // std::size_t

// sessions ?
namespace clients
{
    // TODO singleton (per GroupSession ??)
    class SessionManager
    {
        public:
          // name: client_unique_ptr ?
          using client_ptr = std::unique_ptr< clients::ClientManager >;

        private:
          // TODO? hash map {client_id, ClientManager} ?
          std::list< client_ptr > clients_;
          //std::size_t clients_num_;

        public:
          // arg: max number of clients ?
          SessionManager();
          ~SessionManager();

          // arg: unique_ptr&& ?
          //void addClient(const ClientManager &client);
          void addClient(client_ptr &client_ptr);
          // arg: int clientId ?
          void removeClient(const ClientManager &client);


          //void sendToClient(int clientId) const;
          // arg: int clientId ?
          void sendToAllClientsOtherThan(const ClientManager &client, const messages::Message &message) const;
          void sendToAllClients(const messages::Message &message) const;

          // ret: underlying container's size_type ?
          // TODO? inline implementation ?
          std::size_t getNumberOfClients() const;
    };
}

#endif /*SESSION_MANAGER_H*/