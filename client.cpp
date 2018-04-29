// TODO define's for including C-style headers? (calling conventions?)
#include <sys/types.h>
#include <sys/socket.h>

// htons function
#include <arpa/inet.h> // TODO?? (alternative, instead)
#include <netinet/in.h>

#include <cerrno>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

const uint16_t PORT = 55555;

const uint32_t REMOTE_SRV_ADDR = INADDR_LOOPBACK;
const uint16_t REMOTE_SRV_PORT = 9999;

const std::size_t MSG_BUF_SIZE = 100;

int main(int argc, char** argv)
{
    // create TCP/IP socket
    int sock_fd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK | SOCK_CLOEXEC*/, 0 /*IPPROTO_TCP*/);
    if( sock_fd < 0 )
    {
        std::cerr << "Error creating TCP/IP socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Socket fd: " << sock_fd << std::endl;


    // TODOAPP change for client
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);   // bind to all local interfaces

    if( bind(sock_fd, (struct sockaddr *) &address, sizeof address) != 0 )
    {
        std::cerr << "Error binding name to the socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    int addr_len = sizeof address;
    if( getsockname(sock_fd, (struct sockaddr *) &address, (socklen_t *) &addr_len) != 0 )
    {
        std::cerr << "Error retrieving socket name\n"
                     "Error: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if( addr_len > sizeof address ) // TODO not needed
    {
        std::cerr << "Retrieved local address truncated because of insufficient addr buffer\n"
                     /*"Errno: " << errno*/ << std::endl;
        exit(EXIT_FAILURE);
    }
    if( addr_len != sizeof address )
    {
        std::cerr << "Retrieved local address of wrong format\n"
                     /*"Errno: " << errno*/ << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Socket bound to local port: " << ntohs(address.sin_port) << '\n' <<
                 "Local address: " << inet_ntoa(address.sin_addr) << std::endl;
    std::cout << std::endl;


    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(REMOTE_SRV_PORT);
    remote_address.sin_addr.s_addr = htonl(REMOTE_SRV_ADDR);

    if( connect(sock_fd, (struct sockaddr *) &remote_address, sizeof remote_address) != 0 )
    {
        std::cerr << "Error initiating a connection with the remote socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "## Connection with the server established ##" << std::endl;

    int remote_addr_len = sizeof remote_address;
    if( getpeername(sock_fd, (struct sockaddr *) &remote_address, (socklen_t *) &remote_addr_len) != 0 )
    {
        std::cerr << "Error retrieving peer socket name\n"
                     "Error: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if( remote_addr_len > sizeof remote_address ) // TODO not needed
    {
        std::cerr << "Retrieved peer address truncated because of insufficient addr buffer\n"
                     /*"Errno: " << errno*/ << std::endl;
        exit(EXIT_FAILURE);
    }
    if( remote_addr_len != sizeof remote_address )
    {
        std::cerr << "Retrieved peer address of wrong format\n"
                     /*"Errno: " << errno*/ << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Server:\n"
                 "  Remote address: " << inet_ntoa(remote_address.sin_addr) << '\n' <<
                 "  Remote port: " << ntohs(remote_address.sin_port) << std::endl;


    // SHUT_RD, SHUT_WR, SHUT_RDWR
    if( shutdown(sock_fd, SHUT_WR) != 0 )
    {
        std::cerr << "Error shuting down transmissions on the local socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }


    char message_buffer[MSG_BUF_SIZE];
    ssize_t bytes_received = 0;

    while( true )
    {
        // in loop? (for 1 message, I mean)
        bytes_received = recv(sock_fd, message_buffer, MSG_BUF_SIZE, 0);

        if( bytes_received < 0 )
        {
            std::cerr << "Error receiving a message from the peer\n"
                         "Error: " << std::strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        else if( bytes_received == 0 )
        {
            std::cout << "# The peer has performed an orderly shutdown." << std::endl;
            break;
            //goto CLOSE_SOCKET;
        }
        
        // TODO? does it have sense / is neccessary?
        char last_char = message_buffer[MSG_BUF_SIZE - 1]; // if \0 -> will not be printed anyway
        /*if( message_buffer[MSG_BUF_SIZE - 1] != '\0' )
        {
            last_char = message_buffer[MSG_BUF_SIZE - 1];
            message_buffer[MSG_BUF_SIZE - 1] = '\0';
        }*/
        message_buffer[MSG_BUF_SIZE - 1] = '\0';
        std::cout << bytes_received << " bytes received. Message:\n" <<
                     "> " << message_buffer << last_char << std::endl;    
    }


//CLOSE_SOCKET:
    if( close(sock_fd) != 0 )
    {
        std::cerr << "Error closing socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        // TODO allow script-automated error status code distinction 
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

// TODO
// recv:
// If a message is too long to fit in the supplied buffer, excess bytes may be discarded depending on the type of socket the message is received from. 
