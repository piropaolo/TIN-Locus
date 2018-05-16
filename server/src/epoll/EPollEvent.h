#ifndef LOCUS_EPOLLEVENT_H
#define LOCUS_EPOLLEVENT_H

//namespace {
class EPollEvent {
public:
    EPollEvent() = default;
    EPollEvent(const EPollEvent &) = delete;
    EPollEvent(EPollEvent &&ePollEvent) noexcept;

    virtual ~EPollEvent();

    virtual int getFileDescriptor() const;

    void setFileDescriptor(const int &fileDescriptor);

    virtual void recv() = 0;

    virtual void send() = 0;

private:
    int fileDescriptor = 0;
};
//}


#endif //LOCUS_EPOLLEVENT_H
