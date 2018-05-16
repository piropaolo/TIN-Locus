project(Locus_epoll)

set(SOURCE_FILES
        EPollManager.cpp EPollManager.h
        EPollEvent.cpp EPollEvent.h)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
