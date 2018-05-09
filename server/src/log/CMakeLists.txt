cmake_minimum_required(VERSION 3.10)
#cmake_minimum_required(VERSION 3.5.1)
project(Locus_log)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        Colour.h Colour.cpp
        Logger.h Logger.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})