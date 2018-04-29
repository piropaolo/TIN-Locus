// TODO define's for including C-style headers? (calling conventions?)
#include <sys/types.h>
#include <sys/socket.h>

// htons function
#include <arpa/inet.h> // TODO?? (alternative, instead)
#include <netinet/in.h>

#include <cerrno>
#include <unistd.h>

// NOTE: C++11 required
#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstring>
#include <iostream>

const uint16_t PORT = 55555;

const uint32_t REMOTE_SRV_ADDR = INADDR_LOOPBACK;
const uint16_t REMOTE_SRV_PORT = 9999;

const std::size_t MSG_BUF_SIZE = 100;

void send_message(int socket_fd, const char *message, std::size_t msg_size);

int main(int argc, char** argv)
{
    // create TCP/IP socket
    int sock_fd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK | SOCK_CLOEXEC*/, 0 /*IPPROTO_TCP*/);
    if( sock_fd < 0 )
    {
        std::cerr << "Error creating TCP/IP socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Socket fd: " << sock_fd << std::endl;


    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(REMOTE_SRV_PORT);
    remote_address.sin_addr.s_addr = htonl(REMOTE_SRV_ADDR);

    if( connect(sock_fd, (struct sockaddr *) &remote_address, sizeof remote_address) != 0 )
    {
        std::cerr << "Error initiating a connection with the remote socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }
    // automatically bounds the socket to a random free port or to a usable shared port with the local address set to INADDR_ANY (0.0.0.0)

    std::cout << "## Connection with the server established ##" << std::endl;
    

    int remote_addr_len = sizeof remote_address;
    if( getpeername(sock_fd, (struct sockaddr *) &remote_address, (socklen_t *) &remote_addr_len) != 0 )
    {
        std::cerr << "Error retrieving peer socket name\n"
                     "Error: " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if( remote_addr_len > sizeof remote_address ) // TODO not needed
    {
        std::cerr << "Retrieved peer address truncated because of insufficient addr buffer\n"
                     /*"Errno: " << errno*/ << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if( remote_addr_len != sizeof remote_address )
    {
        std::cerr << "Retrieved peer address of wrong format\n"
                     /*"Errno: " << errno*/ << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Server:\n"
                 "  Remote address: " << inet_ntoa(remote_address.sin_addr) << '\n' <<
                 "  Remote port: " << ntohs(remote_address.sin_port) << std::endl;


    // TODOAPP change for client
    struct sockaddr_in local_address;
    int local_addr_len = sizeof local_address;

    if( getsockname(sock_fd, (struct sockaddr *) &local_address, (socklen_t *) &local_addr_len) != 0 )
    {
        std::cerr << "Error retrieving local socket name\n"
                     "Error: " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if( local_addr_len > sizeof local_address ) // TODO not needed
    {
        std::cerr << "Retrieved local address truncated because of insufficient addr buffer\n"
                     /*"Errno: " << errno*/ << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if( local_addr_len != sizeof local_address )
    {
        std::cerr << "Retrieved local address of wrong format\n"
                     /*"Errno: " << errno*/ << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Local host:\n"
                 "  Local port: " << ntohs(local_address.sin_port) << '\n' <<
                 "  Local address: " << inet_ntoa(local_address.sin_addr) << std::endl;
    std::cout << std::endl;


    // Receive the Welcome Message from the server. Ends with the NULL character.
    char message_buffer[MSG_BUF_SIZE];
    ssize_t bytes_received = 0;
    //bool first_msg_part = true;
    do
    {
        bytes_received = recv(sock_fd, message_buffer, MSG_BUF_SIZE, 0);

        if( bytes_received < 0 )
        {
            std::cerr << "Error receiving a message from the peer\n"
                         "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }
        else if( bytes_received == 0 )
        {
            std::cout << "# The peer has performed an orderly shutdown." << std::endl;
            break;
            //goto CLOSE_SOCKET;
        }      

        std::cout << bytes_received << " bytes received. Message:\n" <<
                     "> ";
        std::cout.write(message_buffer, bytes_received);
        std::cout << std::endl; 
    }
    while( message_buffer[bytes_received - 1] != '\0' ); // not a complete message yet
    // bytes_received <= MSG_BUF_SIZE


    // shut down the receiving part of the full-duplex TCP connection (disallow further receptions from the peer)
    if( shutdown(sock_fd, SHUT_RD) != 0 )
    {
        std::cerr << "Error shuting down transmissions on the local socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }


    std::streamsize characters_read;
    std::cout << std::boolalpha;
    while( std::cin.getline(message_buffer, MSG_BUF_SIZE) ) // delimiter = '\n'
    {
        characters_read = std::cin.gcount();    // count of characters typed in (visible characters + terminating NULL)
                                                // characters_read is always >= 1 (if only the delim character found in the input - the string will consist of only the NULL character)
        characters_read -= 1;                   // do not send the terminating NULL character
        std::cout << "Read: " << characters_read << std::endl;   
        std::cout << "Cin eof:  " << std::cin.eof() << std::endl;
        std::cout << "Cin fail: " << std::cin.fail() << std::endl;
        /*if( characters_read == 1 )              // if only delim char as input - do not send
            continue;*/
        if( characters_read > 0 )
            send_message(sock_fd, message_buffer, characters_read);
    }
    // EOF character (Ctrl + D) terminates the input loop
    /*std::cout << "Cin eof:  " << std::cin.eof() << std::endl;
    std::cout << "Cin fail: " << std::cin.fail() << std::endl;*/


//CLOSE_SOCKET:
    if( close(sock_fd) != 0 )
    {
        std::cerr << "Error closing socket\n"
                     "Error: " << std::strerror(errno) << std::endl;
        // TODO allow script-automated error status code distinction 
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void send_message(int socket_fd, const char *message, std::size_t msg_size)
{
    ssize_t bytes_sent = 0, total_bytes_sent = 0;
    ssize_t msg_ssize = static_cast<ssize_t>(msg_size);

    do
    {
        if( total_bytes_sent > 0 )
        {
            std::cout << "Sending another part of the message.\n"
                         "Bytes sent: " << total_bytes_sent << "\n" <<
                         "Total message size: " << msg_ssize << std::endl;
        }

        bytes_sent = send(socket_fd, message + total_bytes_sent, msg_ssize - total_bytes_sent, 0);

        if( bytes_sent < 0 )
        {
            std::cerr << "Error sending message on socket_fd: " << socket_fd << ".\n"
                         "Error: " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        total_bytes_sent += bytes_sent;
    }
    while( total_bytes_sent < msg_ssize );
    std::cout << "Message sent on socket_fd: " << socket_fd << "\n"
                 "Bytes sent: " << total_bytes_sent << std::endl;
}

// TODO
// recv:
// If a message is too long to fit in the supplied buffer, excess bytes may be discarded depending on the type of socket the message is received from. 
