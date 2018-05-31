project(Locus_buffer_test)

include_directories(${gtest_SOURCE_DIR}/include &{gtest_SOURCE_DIR})

add_executable(${PROJECT_NAME}
        BlockingBuffer_test.cpp
        Buffer_test.cpp
        Converter_test.cpp)

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} Locus_buffer)