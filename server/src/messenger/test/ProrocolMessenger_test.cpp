#include "gtest/gtest.h"
#include "../SimpleMessenger.h"
#include "../ProrocolMessenger.h"

using namespace messenger;

TEST(Constructor_, EmptyQueue) {
    SimpleMessenger simpleMessenger;
    ProrocolMessenger prorocolMessenger(&simpleMessenger);
    EXPECT_TRUE(1);
}