#include "EPollEvent.h"

#include <unistd.h>
#include "log/Logger.h"

//namespace {
    int EPollEvent::getFileDescriptor() const {
        return fileDescriptor;
    }

    void EPollEvent::setFileDescriptor(const int &fileDescriptor) {
        EPollEvent::fileDescriptor = fileDescriptor;
    }

    EPollEvent::~EPollEvent() {
        if(getFileDescriptor() > 0) {
            close(getFileDescriptor());
            log::Logger::getInstance().logMessage("EPollEvent: Close file descriptor: " + std::to_string(getFileDescriptor()));
        }
    }

//}