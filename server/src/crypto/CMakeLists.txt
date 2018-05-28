project(Locus_crypto)

set(SOURCE_FILES
        Crypto.cpp Crypto.h)

add_subdirectory(test)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})