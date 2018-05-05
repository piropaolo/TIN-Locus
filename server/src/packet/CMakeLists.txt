project(Locus/packet)

set(SOURCE_FILES
        PacketType.cpp PacketType.h
#        Packet.cpp Packet.h
        )

add_library(packet STATIC ${SOURCE_FILES})

add_subdirectory(test)