project(Locus_messenger)


set(SOURCE_FILES
        Messenger.cpp Messenger.h
        SimpleMessenger.h SimpleMessenger.cpp
        EncryptMessenger.cpp EncryptMessenger.h
        ProrocolMessenger.cpp ProrocolMessenger.h
        )

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})