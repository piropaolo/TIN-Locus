cmake_minimum_required(VERSION 3.10)
project(Locus)

#find_package(Threads)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_subdirectory(src/log)
add_subdirectory(src/packet)
add_subdirectory(src/message)
add_subdirectory(src/messenger)

add_subdirectory(lib)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        log packet message
#        ${CMAKE_THREAD_LIBS_INIT}
        )