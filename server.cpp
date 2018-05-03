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

// NOTE: C++11 required
//#include <system_error>

#include <cstdlib>
#include <cstring>
#include <iostream>

// TODO temp
#include <list>
#include <memory> // std::make_unique (needs C++14), std::unique_ptr

#include "signal_handlers.h"
#include "SocketEndpoint.h"
#include "ClientManager.h"

const uint16_t LISTEN_PORT = 9999;

const std::size_t MSG_BUF_SIZE = 100;

// TODO? define special, non-standard types if lacking on build platform
/*#ifndef socklen_t
#define socklen_t int
#endif
// typedef int socklen_t;
*/

void close_socket_in_thread(int socket_fd);
extern "C" void * thread_routine(void *connection_socket_fd);    // TODO? TODOCHECK C-language linkage?

// TODO hash(?)-map for threads; key = thread_id
// int thread_exit_status;

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

    // TODO temporary; temporarily here
    std::list< std::unique_ptr< clients::ClientManager > > clients;

    while( true )
    {
        con_sock_fd = listenSocketEndpoint.acceptPendingConnection();
        
        // TODO need new (alloc)
        //comm_layer::CommSocketEndpoint commEndpoint(con_sock_fd);
        
        // TODO cannot pass CommEndpoint by const reference, because close() method is not const !!
        clients.push_back( std::make_unique< clients::ClientManager >( std::make_unique< comm_layer::CommSocketEndpoint >(con_sock_fd) ) );
        // TODO does removing/poping call dtor of the element?
        // TODO dtor of the element shall free the allocated memory -> std::unique_ptr
        clients.back()->startFullDuplexComm();
    }
    
    // TODO now it's never reached:
    std::cout << "--Closing server--" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return EXIT_SUCCESS;
}


// TODO
// SO_REUSEADDR flag:
// A TCP local socket address that has been bound is unavailable for some time after closing, unless the SO_REUSEADDR flag has been set. Care should be taken when using this flag as it makes TCP less reliable. 

// TODO
// accept errno
// Linux accept() (and accept4()) passes already-pending network errors on the new socket as an error code from accept(). This behavior differs from other BSD socket implementations. For reliable operation the application should detect the network errors defined for the protocol after accept() and treat them like EAGAIN by retrying. In the case of TCP/IP, these are ENETDOWN, EPROTO, ENOPROTOOPT, EHOSTDOWN, ENONET, EHOSTUNREACH, EOPNOTSUPP, and ENETUNREACH. 

// TODOSYNC
// close socket
// It is probably unwise to close file descriptors while they may be in use by system calls in other threads in the same process. Since a file descriptor may be reused, there are some obscure race conditions that may cause unintended side effects. 

// TODO
// blocking send call:
// TODO
// nonblocking socket send + select:
// When the message does not fit into the send buffer of the socket, send() normally blocks, unless the socket has been placed in nonblocking I/O mode. In nonblocking mode it would fail with the error EAGAIN or EWOULDBLOCK in this case. The select(2) call may be used to determine when it is possible to send more data.

// TODOSEC
// close open sockets after each error. or do it automatically (SOCK_CLOEXEC flag)

// TODO
// C++ chrono clock:
// The standard recommends that a steady clock is used to measure the duration. If an implementation uses a system clock instead, the wait time may also be sensitive to clock adjustments.

// TODOWARY
// exit function:
// Stack is not unwound - destructors of variables with automatic storage duration are not called. 

// TODOWARY
// closing connection:
// SO_LINGER socket option:
// When enabled, a close(2) or shutdown(2) will not return until all queued messages for the socket have been successfully sent or the linger timeout has been reached. Otherwise, the call returns immediately and the closing is done in the background. When the socket is closed as part of exit(2), it always lingers in the background.

// TODOWARY, TODOTHREAD
// inet_ntoa
// strerror
// strsignal
// are not thread-safe functions (cannot be safely called from multiple threads at the same time)

// TODOTHREAD
// thread terminates when:
// Any of the threads in the process calls exit(3), or the main thread performs a return from main(). This causes the termination of all threads in the process.

// TODOTHREAD
// main thread:
// To allow other threads to continue execution, the main thread should terminate by calling pthread_exit() rather than exit(3).
// BUGS: 
// Currently, there are limitations in the kernel implementation logic for wait(2)ing on a stopped thread group with a dead thread group leader. This can manifest in problems such as a locked terminal if a stop signal is sent to a foreground process whose thread group leader has already called pthread_exit(). 

// TODOTHREAD
// pthread_exit:
// Performing a return from the start function of any thread other than the main thread results in an implicit call to pthread_exit(), using the function's return value as the thread's exit status.

// TODOTHREAD
// thread join:
// A thread that is created in a joinable state should eventually either be joined using pthread_join(3) or detached using pthread_detach(3); see pthread_create(3).
// If multiple threads simultaneously try to join with the same thread, the results are undefined. If the thread calling pthread_join() is canceled, then the target thread will remain joinable (i.e., it will not be detached).
// After a successful call to pthread_join(), the caller is guaranteed that the target thread has terminated.
// Joining with a thread that has previously been joined results in undefined behavior.
// Failure to join with a thread that is joinable (i.e., one that is not detached), produces a "zombie thread". Avoid doing this, since each zombie thread consumes some system resources, and when enough zombie threads have accumulated, it will no longer be possible to create new threads (or processes).
// There is no pthreads analog of waitpid(-1, &status, 0), that is, "join with any terminated thread". If you believe you need this functionality, you probably need to rethink your application design.
// All of the threads in a process are peers: any thread can join with any other thread in the process.
// JOINABLE VS DETACHED THREAD:
// A thread may either be joinable or detached. If a thread is joinable, then another thread can call pthread_join(3) to wait for the thread to terminate and fetch its exit status. Only when a terminated joinable thread has been joined are the last of its resources released back to the system. When a detached thread terminates, its resources are automatically released back to the system: it is not possible to join with the thread in order to obtain its exit status. Making a thread detached is useful for some types of daemon threads whose exit status the application does not need to care about. By default, a new thread is created in a joinable state, unless attr was set to create the thread in a detached state (using pthread_attr_setdetachstate(3)).
// Either pthread_join(3) or pthread_detach() should be called for each thread that an application creates, so that system resources for the thread can be released. (But note that the resources of all threads are freed when the process terminates.)  

// TODOTHREAD
// detached thread:
// It is an error to specify the thread ID of a thread that was created in a detached state in a later call to pthread_detach(3) or pthread_join(3).
// The detached attribute merely determines the behavior of the system when the thread terminates; it does not prevent the thread from being terminated if the process terminates using exit(3) (or equivalently, if the main thread returns). 

// TODOTHREAD
// thread cancellation request:
// After a canceled thread has terminated, a join with that thread using pthread_join(3) obtains PTHREAD_CANCELED as the thread's exit status. (Joining with a thread is the only way to know that cancellation has completed.)

// TODOWARY, TODOTHREAD
// thread id:
// POSIX.1 allows an implementation wide freedom in choosing the type used to represent a thread ID; for example, representation using either an arithmetic type or a structure is permitted. Therefore, variables of type pthread_t can't portably be compared using the C equality operator (==); use pthread_equal(3) instead.
// Thread identifiers should be considered opaque: any attempt to use a thread ID other than in pthreads calls is nonportable and can lead to unspecified results.
// The thread ID returned by pthread_self() is not the same thing as the kernel thread ID returned by a call to gettid(2).

// TODOSYNC, TODOSIGNAL, TODOCHECK
// If a signal is delivered to a thread waiting for a mutex, upon return from the signal handler the thread shall resume waiting for the mutex as if it was not interrupted.


// nonblocking accept:
// If no pending connections are present on the queue, and the socket is not marked as nonblocking, accept() blocks the caller until a connection is present. If the socket is marked nonblocking and no pending connections are present on the queue, accept() fails with the error EAGAIN or EWOULDBLOCK. 
// select, poll, SIGIO:
// In order to be notified of incoming connections on a socket, you can use select(2) or poll(2). A readable event will be delivered when a new connection is attempted and you may then call accept() to get a socket for that connection. Alternatively, you can set the socket to deliver SIGIO when activity occurs on a socket; see socket(7) for details.
// accept after SIGIO/select/poll:
// There may not always be a connection waiting after a SIGIO is delivered or select(2) or poll(2) return a readability event because the connection might have been removed by an asynchronous network error or another thread before accept() is called. If this happens then the call will block waiting for the next connection to arrive. To ensure that accept() never blocks, the passed socket sockfd needs to have the O_NONBLOCK flag set (see socket(7)).
// explicitly set flags:
// On Linux, the new socket returned by accept() does not inherit file status flags such as O_NONBLOCK and O_ASYNC from the listening socket. This behavior differs from the canonical BSD sockets implementation. Portable programs should not rely on inheritance or noninheritance of file status flags and always explicitly set all required flags on the socket returned from accept().  

// accept4(sock_fd, &peer_address, &peer_addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC)
// nonblocking accept, accept4 is a nonstandard Linux extension

// nonblocking recv:
// If no messages are available at the socket, the receive calls wait for a message to arrive, unless the socket is nonblocking (see fcntl(2)), in which case the value -1 is returned and the external variable errno is set to EAGAIN or EWOULDBLOCK. The receive calls normally return any data available, up to the requested amount, rather than waiting for receipt of the full amount requested.
// The select(2) or poll(2) call may be used to determine when more data arrives. 