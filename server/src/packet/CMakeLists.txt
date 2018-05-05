project(Locus/packet)

set(SOURCE_FILES
        PacketType.cpp PacketType.h
        Packet.cpp Packet.h
        Converter.h)

add_subdirectory(test)

add_library(packet STATIC ${SOURCE_FILES})
