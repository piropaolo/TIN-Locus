#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*extern const char sigint_handler_msg[];  // just declaration
extern const char sigterm_handler_msg[];
extern const char sigquit_handler_msg[];
extern const char sigtstp_handler_msg[];
extern const char sigcont_handler_msg[];*/
// not needed outside of signal_handlers.cpp

// NOTE: POSIX requires that signal is thread-safe, and specifies a list of async-signal-safe library functions that may be called from any signal handler. 
// Signal handlers are expected to have C linkage and, in general, only use the features from the common subset of C and C++.
void signal_handler(int signal);

#ifdef __cplusplus
}
#endif

#endif /*SIGNAL_HANDLERS_H*/