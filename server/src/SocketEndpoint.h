//#pragma once
#ifndef SOCKET_ENDPOINT_H
#define SOCKET_ENDPOINT_H

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
  #include <stdint.h>
}
//or: #include <cstdint> // => std::uint16_t instead of (::)uint16_t

#include <cstring>

namespace comm_layer
{
    //using sockaddr_in = struct sockaddr_in; // TODO? create a simpler structure?

    class CommSocketEndpoint
    {
        private:
          int socket_fd_;
          /*mutable ??? -> close method could be const*/
          bool socket_open_;

        public:
          CommSocketEndpoint(const int socket_fd);
          ~CommSocketEndpoint();

          // TODO? subclasses Sender, Receiver ? -- one for each thread (SenderThread, ReceiverThread)
          // TODO which thread should close the connection when server initiates knock-off time (end-of-work)?
          // can sending and receiving be done simultaneously on one socket? Yup, idgit ¬_¬

          // TODO? throw specifications?
          // TODO in every method check if socket_open_ (?)
          void sendNBytes(const char *message, std::size_t message_size) const; // TODO? const void *message
          void receiveNBytes(char *buffer, std::size_t buffer_size) const;

          /** Returns number of bytes actually received and written into buffer. **/
          /*ssize_t ?*/ int receive(char *buffer, std::size_t max_bytes) const;

          // TODO? add send(messages::Message)

          void close();
    };

    // TODO? singleton?
    class ListenSocketEndpoint // : public SocketEndpoint ?
    {
        private:
          int socket_fd_;
          /*const*/ ::uint16_t port_number_;

        //private:
        //  const int SOCKET_OPTION_ON_ = 1;

        public:
          const int LISTEN_TCP_SOCKET_BACKLOG_SIZE = 5;

        public:
          ListenSocketEndpoint(const uint16_t port_number);
          ~ListenSocketEndpoint();
          // RAII
        
          /*CommSocketEndpoint*/ int acceptPendingConnection() const;
          // TODO acceptPendingConnections -- loop inside + registered callback function (invoked for every new connection with CommSocketEndpoint as argument)
          // TODO? return reference to new CommSocketEndpoint (alloc and free via smart pointers / stored in a collection (push_back-ed))

          // TODO configurable socket options -- pass structure SocketConfig to ctor / to static class member function (to set for following created sockets)?

          //sockaddr_in& getSocketEndpointAddress() const; // TODO use automatic memory alloc and free with smart pointers
          // temporary substitute, meh ¬_¬
          void printSocketEndpointAddress() const;

          // TODO
          //void close();
          //void endListening();
    };
}

#endif /*SOCKET_ENDPOINT_H*/