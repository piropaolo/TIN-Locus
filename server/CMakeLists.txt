cmake_minimum_required(VERSION 3.10)
project(Locus)

#find_package(Threads)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_subdirectory(src/log)

add_executable(Locus src/main.cpp)

target_link_libraries(Locus
        log
#        ${CMAKE_THREAD_LIBS_INIT}
        )