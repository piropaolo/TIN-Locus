project(Locus_messenger_test)

include_directories(${gtest_SOURCE_DIR}/include &{gtest_SOURCE_DIR})

add_executable(${PROJECT_NAME}
        SimpleMessenger_test.cpp
        EncryptMessenger_test.cpp
        ProrocolMessenger_test.cpp
        )

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} messenger packet)