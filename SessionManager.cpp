#include "SessionManager.h"

#include <utility>

// DBG
//#include <cassert>

namespace clients
{
    SessionManager::SessionManager()
    {}
    SessionManager::~SessionManager()
    {}

    void SessionManager::addClient(/*const*/ client_ptr &client_ptr)
    {
        clients_.push_back( std::move(client_ptr) );
    }

    void SessionManager::removeClient(const ClientManager &client)
    {
        auto previous_size = clients_.size();
        clients_.remove_if( 
            [&client](const client_ptr &element_ptr)
            {
                return client == *element_ptr;
            } 
        );

        // Reliably can be only done in a critical section:
        // on addClient, removeClient (called from different threads, possibly at the same moment in time)
        // DBG
        //assert(previous_size == clients_.size() + 1);
    }

    void SessionManager::sendToAllClientsOtherThan(const ClientManager &client, const messages::Message &message) const
    {
        // TODO? for_each
        for(auto& clientptr : clients_)
            if( *clientptr != client )
                clientptr->send(message);   // TODO std::move ?
    }

    void SessionManager::sendToAllClients(const messages::Message &message) const
    {
        for(auto& clientptr : clients_)
                clientptr->send(message);
    }

    std::size_t SessionManager::getNumberOfClients() const
    {
        return clients_.size();
    }
}