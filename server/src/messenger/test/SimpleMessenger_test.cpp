#include "gtest/gtest.h"
#include "../SimpleMessenger.h"

using namespace messenger;

TEST(SimpleMessenger_Constructor, Empty) {
    SimpleMessenger simpleMessenger;
    EXPECT_TRUE(1);
}