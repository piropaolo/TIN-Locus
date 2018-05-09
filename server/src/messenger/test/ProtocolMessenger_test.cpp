#include "gtest/gtest.h"
#include "../SimpleMessenger.h"
#include "messenger/ProtocolMessenger.h"

using namespace messenger;

TEST(ProtocolMessenger_Constructor, EmptyQueue) {
    SimpleMessenger simpleMessenger;
    ProtocolMessenger prorocolMessenger(&simpleMessenger);
    EXPECT_TRUE(1);
}