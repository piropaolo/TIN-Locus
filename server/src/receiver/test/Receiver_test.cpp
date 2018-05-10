#include "gtest/gtest.h"
#include "receiver/ConnectionManager.h"

using namespace receiver;

TEST(Receiver_Constructor, Initialize) {
    ConnectionManager receiver;
    EXPECT_EQ(1, 1);
}
