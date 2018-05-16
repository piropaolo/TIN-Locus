project(Locus_server)

set(SOURCE_FILES
        BasicThread.cpp BasicThread.h
        Receiver.cpp Receiver.h
        Sender.cpp Sender.h
        Server.cpp Server.h
        Acceptor.cpp Acceptor.h
        ClientManager.cpp ClientManager.h
        )

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
