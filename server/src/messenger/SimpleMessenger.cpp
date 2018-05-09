#include "SimpleMessenger.h"

#include "packet/Converter.h"

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
}

#include <cerrno>
// NOTE: C++11 required
#include <system_error>

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <sstream>
#include "log/Logger.h"

using namespace packet;

namespace messenger {

    void SimpleMessenger::send(const void *buffer, std::size_t n) {
        // TODO? memcpy message? - can the message buffer be modified during this function execution?
        // TODO? or serialize string? / string's buffer? -- string as func argument
        ssize_t bytes_sent = 0, total_bytes_sent = 0; //characters? (unicode?)
        ssize_t msg_ssize = static_cast<ssize_t>(n);

        do
        {
            bytes_sent = send(socketFd_, buffer + total_bytes_sent, (size_t)(msg_ssize - total_bytes_sent), 0);

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

    void SimpleMessenger::recv(void *buffer, std::size_t n) {
        // TODO? operate on local buffer (dynamically allocated with size buffer_size) and then memcpy to buffer (only when buffer_size bytes received) ?
        //char message_buffer[MSG_BUF_SIZE];
        ssize_t bytes_received = 0, total_bytes_received = 0;
        ssize_t buffer_ssize = static_cast<ssize_t>(n);

        do
        {
            bytes_received = recv(socketFd_, buffer + total_bytes_received, (size_t)(buffer_ssize - total_bytes_received), 0);

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

    std::size_t SimpleMessenger::receiveAny(void *buffer, std::size_t max_bytes) {
        ssize_t bytes_received = 0;

        bytes_received = recv(socketFd_, buffer, max_bytes, 0);

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

        return static_cast<std::size_t>(bytes_received);
    }

    void SimpleMessenger::init() {
        Messenger::send(Packet(PacketType::OPEN));
    }

    /** explicitly terminate the communication (close connection) **/
    void SimpleMessenger::close() {
        if( socketOpen_ )
        {
            if( ::close(socketFd_) != 0 )
            {
                std::cerr << "Error closing socket. Socket_fd: " << socket_fd_ << "\n"
                             "Error: " << std::strerror(errno) << std::endl;

                throw std::system_error(errno, std::generic_category());
            }

            socketOpen_ = false;
        }
    }


    SimpleMessenger::SimpleMessenger(int socket_fd) : socketFd_(socket_fd), socketOpen_(true) {}

    SimpleMessenger::~SimpleMessenger() {
        // TODOCHECK check whether this works with threads in 100% situations
        if( socketOpen_ )
        {
            // TODO general macros for checking error conditions + throwing exceptions?
            if( ::close(socketFd_) != 0 )
            {
                std::cerr << "Error closing socket. Socket_fd: " << socket_fd_ << "\n"
                             "Error: " << std::strerror(errno) << std::endl;

                throw std::system_error(errno, std::generic_category());
            }

            socketOpen_ = false;
        }
    }
}