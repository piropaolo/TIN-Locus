#include "gtest/gtest.h"
#include "../SimpleMessenger.h"

using namespace messenger;

TEST(SimpleMessenger_, Empty) {
    SimpleMessenger simpleMessenger;
    EXPECT_TRUE(1);
}