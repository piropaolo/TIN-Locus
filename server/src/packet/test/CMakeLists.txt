project(Locus_packet_test)

include_directories(${gtest_SOURCE_DIR}/include &{gtest_SOURCE_DIR})

add_executable(${PROJECT_NAME}
        PacketType_test.cpp
        Converter_test.cpp
        BlockBuffer_test.cpp
        Packet_test.cpp)

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} packet)