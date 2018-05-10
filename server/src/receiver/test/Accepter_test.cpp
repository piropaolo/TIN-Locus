#include "gtest/gtest.h"
#include "receiver/Acceptor.h"

using namespace receiver;

TEST(Accepter_Constructor, Initialize) {
    Acceptor accepter(8080);
    EXPECT_EQ(1, 1);
}
