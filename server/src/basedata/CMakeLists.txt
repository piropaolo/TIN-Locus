project(Locus_basedata)

set(SOURCE_FILES
        Position.cpp Position.h
        Watcher.cpp Watcher.h
        ClientInfo.cpp ClientInfo.h
        ClientDataManager.cpp ClientDataManager.h)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
