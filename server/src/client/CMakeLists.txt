project(Locus_client)

set(SOURCE_FILES
        ClientBuffer.cpp ClientBuffer.h
        Client.cpp Client.h
        SimpleClient.cpp SimpleClient.h
        DecoratorClient.cpp DecoratorClient.h
        EncryptClient.cpp EncryptClient.h
        ProtocolClient.cpp ProtocolClient.h)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
