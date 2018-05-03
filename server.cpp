extern "C" { // avoid C++ name mangling; in fact not needed (over-protective?) as these are system libraries with use in C++ code in mind (no doubt)
  #include <sys/types.h>
}

extern "C" {
  #include <unistd.h> // standard symbolic constants and types (e.g. STDOUT_FILENO)
}

#include <csignal>

// NOTE: C++11 required
#include <thread>
#include <chrono>
// NOTE: for std::literals::chrono_literals C++14 required

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <memory> // std::make_unique (needs C++14), std::unique_ptr

#include "signal_handlers.h"
#include "SocketEndpoint.h"
#include "SessionManager.h"
#include "ClientManager.h"

const uint16_t LISTEN_PORT = 9999;

// TODO hash(?)-map for threads; key = thread_id


// TODO temporary; temporarily here; temporary solution: need singleton (?)
clients::SessionManager session_manager;

int main(int argc, char** argv)
{
    // Install signal handlers
    // The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.
    if( std::signal(SIGINT, signal_handler) == SIG_ERR ) // terminal interrupt signal, ctrl + c
    {
        // Setting a signal handler can be disabled on some implementations.
        std::cerr << "Error installing the SIGINT signal handler\n"
                     /*<< "Error: " << std::strerror(errno)*/;
        std::exit(EXIT_FAILURE);
    }
    if( std::signal(SIGTERM, signal_handler) == SIG_ERR ) // termination request
    {
        std::cerr << "Error installing the SIGTERM signal handler\n";
        std::exit(EXIT_FAILURE);
    }
    if( std::signal(SIGQUIT, signal_handler) == SIG_ERR ) // terminal quit signal, ctrl + backslash
    {
        std::cerr << "Error installing the SIGQUIT signal handler\n";
        std::exit(EXIT_FAILURE);
    }
    if( std::signal(SIGTSTP, signal_handler) == SIG_ERR ) // terminal stop signal, ctrl + z
    {
        std::cerr << "Error installing the SIGTSTP signal handler\n";
        std::exit(EXIT_FAILURE);
    }
    if( std::signal(SIGCONT, signal_handler) == SIG_ERR ) // continue executing, if stopped
    {
        std::cerr << "Error installing the SIGCONT signal handler\n";
        std::exit(EXIT_FAILURE);
    }

    comm_layer::ListenSocketEndpoint listenSocketEndpoint(LISTEN_PORT);
    listenSocketEndpoint.printSocketEndpointAddress();

    int con_sock_fd;

    while( true )
    {
        con_sock_fd = listenSocketEndpoint.acceptPendingConnection();

        // TODO simplify and encapsulate (hide) these:
        auto commEndpoint = std::make_unique< comm_layer::CommSocketEndpoint >(con_sock_fd);
        auto client = std::make_unique< clients::ClientManager >( std::move(commEndpoint) ); // TODO std::move ?

        auto client_temp_ptr = client.get();

        session_manager.addClient(client); // TODO std::move ?

        client_temp_ptr->startFullDuplexComm();
        
        // dtor of the element shall free the allocated memory => std::unique_ptr
    }
    
    // TODO now it's never reached:
    std::cout << "--Closing server--" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return EXIT_SUCCESS;
}
