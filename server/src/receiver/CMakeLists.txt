project(Locus_receiver)

set(SOURCE_FILES
        ConnectionManager.cpp ConnectionManager.h
        Buffer.cpp Buffer.h
        BufferManager.cpp BufferManager.h
        EPollEvent.cpp EPollEvent.h
        Acceptor.cpp Acceptor.h ClientBuffer.cpp ClientBuffer.h)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
