project(Locus_packet)

set(SOURCE_FILES
        PacketType.cpp PacketType.h
        Packet.cpp Packet.h
        Converter.h BlockBuffer.cpp BlockBuffer.h)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
