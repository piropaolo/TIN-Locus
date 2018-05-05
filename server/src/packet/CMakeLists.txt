cmake_minimum_required(VERSION 3.10)
project(Locus/packet)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        PacketType.cpp PacketType.h
        )

add_library(packet STATIC ${SOURCE_FILES})