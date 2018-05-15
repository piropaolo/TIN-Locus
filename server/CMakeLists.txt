cmake_minimum_required(VERSION 3.10)
project(Locus)

find_package(Threads)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

include_directories(src)

add_subdirectory(src/client)
add_subdirectory(src/log)
add_subdirectory(src/packet)
add_subdirectory(src/message)
add_subdirectory(src/messenger)
add_subdirectory(src/server)

add_subdirectory(lib)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        Locus_server Locus_log Locus_packet Locus_message Locus_messenger
        ${CMAKE_THREAD_LIBS_INIT}
        )