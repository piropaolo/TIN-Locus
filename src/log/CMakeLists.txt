cmake_minimum_required(VERSION 3.10)
project(Locus/log)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        Colour.h Colour.cpp
        Logger.h Logger.cpp)

add_library(log STATIC ${SOURCE_FILES})