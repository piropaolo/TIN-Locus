#ifndef LOCUS_PIPEBUFFER_H
#define LOCUS_PIPEBUFFER_H

#include <unistd.h> // pipe

class PipeBuffer {
public:
    PipeBuffer();

//    PipeBuffer(PipeBuffer&&);
    PipeBuffer(const PipeBuffer &) = delete;

    int getReadFd() const;

    int getWriteFd() const;

    void pushByteToPipe() const;

    void popByteFromPipe() const;

private:
    /*mutable*/ union {
        struct {
            int pipeReadFd;
            int pipeWriteFd;
        };
        int pipeFds[2];
    };
};


#endif //LOCUS_PIPEBUFFER_H
