#include "signal_handlers.h" // TODO? TODOCHECK

extern "C" {
  #include <unistd.h> // standard symbolic constants and types (e.g. STDOUT_FILENO)
}

#include <csignal> // TODOCHECK TODO? can we use std:: functions after including signal.h??
extern "C" {
  #include <signal.h> // POSIX library, more portable sigaction function
}

// NOTE: for use of 'sizeof xxx_msg' xxx_msg must be declared as of type char[], not char*
const char sigint_handler_msg[] = "@ SIGINT catched @";
const char sigterm_handler_msg[] = "@ SIGTERM catched @";
const char sigquit_handler_msg[] = "@ SIGQUIT catched @";
const char sigtstp_handler_msg[] = "@ SIGTSTP catched @";
const char sigcont_handler_msg[] = "@ SIGCONT catched @";

void signal_handler(int signal)
{
    if( signal == SIGINT )
        // returns number of bytes written or -1 and sets errno
        write(STDOUT_FILENO, (const void *) sigint_handler_msg, sizeof sigint_handler_msg - 1); // without the terminating '\0'
    else if( signal == SIGTERM )
        write(STDOUT_FILENO, (const void *) sigterm_handler_msg, sizeof sigterm_handler_msg - 1);
    else if( signal == SIGQUIT )
        write(STDOUT_FILENO, (const void *) sigquit_handler_msg, sizeof sigquit_handler_msg - 1);
    else if( signal == SIGTSTP )
        write(STDOUT_FILENO, (const void *) sigtstp_handler_msg, sizeof sigtstp_handler_msg - 1);
    else if( signal == SIGCONT )
        write(STDOUT_FILENO, (const void *) sigcont_handler_msg, sizeof sigcont_handler_msg - 1);

    // TODO? do clean-up ?

    std::signal(signal, SIG_DFL); // restore the default handler
    ::raise(signal); // NOTE: important, not std::raise; raise from signal.h
    //std::cout << "Raise has returned" << std::endl;
    // TODO? is raise a good idea?
    // TODOCPP how to call raise from signal.h and not csignal, at the same time calling std::signal from csignal and not signal.h??
}