project(Locus_server)

set(SOURCE_FILES
        EPollManager.cpp EPollManager.h
        Buffer.cpp Buffer.h
        EPollEvent.cpp EPollEvent.h
        Acceptor.cpp Acceptor.h
        ClientBuffer.cpp ClientBuffer.h
        BasicThread.cpp BasicThread.h
        Receiver.cpp Receiver.h
        Sender.cpp Sender.h
        Server.cpp Server.h
        ClientManager.cpp ClientManager.h
        Client.cpp Client.h)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
