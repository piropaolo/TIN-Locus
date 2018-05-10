project(Locus_receiver_test)

include_directories(${gtest_SOURCE_DIR}/include &{gtest_SOURCE_DIR})

add_executable(${PROJECT_NAME}
        Buffer_test.cpp
        Accepter_test.cpp Receiver_test.cpp)

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} Locus_receiver Locus_message Locus_log)