#include "gtest/gtest.h"
#include "../EncryptMessenger.h"
#include "../SimpleMessenger.h"

using namespace messenger;

TEST(EncryptMessenger, Empty) {
    SimpleMessenger simpleMessenger;
    EncryptMessenger encryptMessenger(&simpleMessenger);
    EXPECT_TRUE(1);
}