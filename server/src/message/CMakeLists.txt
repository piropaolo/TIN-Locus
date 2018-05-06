project(Locus/message)

set(SOURCE_FILES
        Message.cpp Message.h
        ErrorType.h
        BlockingQueue.h
        )

add_subdirectory(test)

add_library(message STATIC ${SOURCE_FILES})