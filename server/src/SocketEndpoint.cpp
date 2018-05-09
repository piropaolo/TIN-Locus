extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>

  // htons function
  #include <arpa/inet.h> // TODO?? (alternative, instead)
  #include <netinet/in.h>
}

#include <cerrno>

// NOTE: C++11 required
#include <system_error>

#include <cstdlib>
#include <cstring>

#include <iostream>

#include "SocketEndpoint.h"

#include <sstream>
#include "log/Logger.h"

namespace comm_layer
{
    CommSocketEndpoint::CommSocketEndpoint(const int socket_fd) : socket_fd_(socket_fd), socket_open_(true)
    {}

    CommSocketEndpoint::~CommSocketEndpoint()   // TODOWARY, TODOCHECK, TODO RAII may not work with threads (pthread_exit will not cause destructors to be called, ?)
    {
        // TODOCHECK check whether this works with threads in 100% situations
        if( socket_open_ )
        {
            // TODO general macros for checking error conditions + throwing exceptions?
            if( ::close(socket_fd_) != 0 )
            {
                std::cerr << "Error closing socket. Socket_fd: " << socket_fd_ << "\n"
                             "Error: " << std::strerror(errno) << std::endl;

                throw std::system_error(errno, std::generic_category());
            }

            socket_open_ = false;
        }
    }

    void CommSocketEndpoint::sendNBytes(const char *message, std::size_t message_size) const
    {
        // TODO? memcpy message? - can the message buffer be modified during this function execution?
        // TODO? or serialize string? / string's buffer? -- string as func argument
        ssize_t bytes_sent = 0, total_bytes_sent = 0; //characters? (unicode?)
        ssize_t msg_ssize = static_cast<ssize_t>(message_size);

        do
        {
            bytes_sent = send(socket_fd_, message + total_bytes_sent, (size_t)(msg_ssize - total_bytes_sent), 0);

            if( bytes_sent < 0 )
            {
                // throw error condition that corresponds to the POSIX errno code
                throw std::system_error(errno, std::generic_category());
            }

            total_bytes_sent += bytes_sent;
        }
        while( total_bytes_sent < msg_ssize );

        std::ostringstream log_stream;
        log_stream << "Bytes send: " << total_bytes_sent;
        log::Logger::getInstance().logDebug( log_stream.str() );
    }

    void CommSocketEndpoint::receiveNBytes(char *buffer, std::size_t buffer_size) const
    {
        // TODO? operate on local buffer (dynamically allocated with size buffer_size) and then memcpy to buffer (only when buffer_size bytes received) ?
        //char message_buffer[MSG_BUF_SIZE];
        ssize_t bytes_received = 0, total_bytes_received = 0;
        ssize_t buffer_ssize = static_cast<ssize_t>(buffer_size);

        do
        {
            bytes_received = recv(socket_fd_, buffer + total_bytes_received, (size_t)(buffer_ssize - total_bytes_received), 0);

            if( bytes_received < 0 )
            {
                std::cerr << "Error receiving a message from the peer. Socket_fd: " << socket_fd_ << "\n"
                             "Error: " << std::strerror(errno) << std::endl;
                throw std::system_error(errno, std::generic_category());
            }
            else if( bytes_received == 0 )
            {
                std::cout << "# The peer has performed an orderly shutdown. Socket_fd: " << socket_fd_ << std::endl;
                break;
                // TODO throw exception (connection shutdown/closed)
            }  

            total_bytes_received += bytes_received;
        }
        while( total_bytes_received < buffer_ssize );
        std::cout << "Bytes received: " << total_bytes_received << std::endl;
    }

    int CommSocketEndpoint::receive(char *buffer, std::size_t max_bytes) const
    {
        ssize_t bytes_received = 0;

        bytes_received = recv(socket_fd_, buffer, max_bytes, 0);

        if( bytes_received < 0 )
        {
            std::cerr << "Error receiving a message from the peer. Socket_fd: " << socket_fd_ << "\n"
                        "Error: " << std::strerror(errno) << std::endl;
            throw std::system_error(errno, std::generic_category());
        }
        else if( bytes_received == 0 )
        {
            std::cout << "# The peer has performed an orderly shutdown. Socket_fd: " << socket_fd_ << std::endl;
            // TODO throw exception (connection shutdown/closed)

            // NOT: throw std::system_error(errno, std::generic_category());
        }  

        return static_cast<int>(bytes_received);
    }

    // explicitly terminate the communication (close connection)
    void CommSocketEndpoint::close()
    {
        if( socket_open_ )
        {
            if( ::close(socket_fd_) != 0 )
            {
                std::cerr << "Error closing socket. Socket_fd: " << socket_fd_ << "\n"
                             "Error: " << std::strerror(errno) << std::endl;

                throw std::system_error(errno, std::generic_category());
            }

            socket_open_ = false;
        }
    }

    // TODO, TODOC++ handle errors in a nice way; exceptions are teh powarrr!!!!
    ListenSocketEndpoint::ListenSocketEndpoint(const uint16_t port_number)
    {
        // create TCP/IP socket
        int socket_fd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK | SOCK_CLOEXEC*/, 0 /*IPPROTO_TCP*/);
        if( socket_fd < 0 )
        {
            std::cerr << "Error creating TCP/IP socket\n"
                        "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        socket_fd_ = socket_fd;

        const int SOCKET_OPTION_ON = 1;
        if( setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, (const void *) &SOCKET_OPTION_ON, sizeof SOCKET_OPTION_ON) != 0 )
        {
            std::cerr << "Error setting the SO_REUSEADDR socket option\n"
                        "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }
        // TODO? assert on getsockopt == true?


        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(port_number);
        address.sin_addr.s_addr = htonl(INADDR_ANY);   // bind to all local interfaces

        if( bind(socket_fd_, (struct sockaddr *) &address, sizeof address) != 0 )
        {
            std::cerr << "Error binding name to the socket\n"
                        "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // binding successful
        port_number_ = port_number;


        if( listen(socket_fd_, LISTEN_TCP_SOCKET_BACKLOG_SIZE) != 0 )
        {
            std::cerr << "Error listening for connections on the socket\n"
                        "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    ListenSocketEndpoint::~ListenSocketEndpoint()
    {
        // TODO? check if the fd was open? (socket created)
        if( ::close(socket_fd_) != 0 )
        {
            std::cerr << "Error closing listen socket\n"
                            "Error: " << std::strerror(errno) << std::endl;
            // TODO allow script-automated error status code distinction 
            std::exit(EXIT_FAILURE);
        }

        std::cout << "Listen socket closed" << std::endl;
    }

    // TODO return some object, Goddammit!
    /*CommSocketEndpoint*/ int ListenSocketEndpoint::acceptPendingConnection() const
    {
        struct sockaddr_in peer_address;
        int peer_addr_len = sizeof peer_address;

        int con_sock_fd = accept(socket_fd_, (struct sockaddr *) &peer_address, (socklen_t *) &peer_addr_len);

        if( con_sock_fd < 0 )
        {
            std::cerr << "Error accepting a connection on the socket\n"
                            "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
            // TODO display message and continue;
        }
        if( peer_addr_len != sizeof peer_address )
        {
            std::cerr << "Peer address of wrong format\n"
                        /*"Errno: " << errno*/ << std::endl;
            std::exit(EXIT_FAILURE);
            // TODO close socket and continue;
        }        

        // TODO move to ConnSocketEndpoint.getPeerAddress
        std::cout << "Connected peer:\n"
                    "  Remote address: " << inet_ntoa(peer_address.sin_addr) << "\n" <<
                    "  Remote port:    " << ntohs(peer_address.sin_port) << "\n"
                    "  socket_fd: " << con_sock_fd << std::endl;
        std::cout << std::endl;

        return con_sock_fd; // TODO return (reference to) (newly allocated and created) ConnSocketEndpoint
    }

    // temporary solution; should return some structure
    void ListenSocketEndpoint::printSocketEndpointAddress() const
    {
        struct sockaddr_in address;
        int addr_len = sizeof address;

        if( getsockname(socket_fd_, (struct sockaddr *) &address, (socklen_t *) &addr_len) != 0 )
        {
            std::cerr << "Error retrieving socket name\n"
                            "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if( addr_len != sizeof address )
        {
            std::cerr << "Retrieved local address of wrong format\n"
                        /*"Errno: " << errno*/ << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::cout << "Socket bound to local port: " << ntohs(address.sin_port) << '\n' <<
                        "Local address: " << inet_ntoa(address.sin_addr) << std::endl;
        std::cout << std::endl;
    }
}