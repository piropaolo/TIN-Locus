project(Locus_message)

set(SOURCE_FILES
        Message.cpp Message.h
        ErrorType.h
        BlockingQueue.h
        )

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})