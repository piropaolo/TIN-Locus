#include "gtest/gtest.h"
#include "../Message.h"

using namespace message;

TEST(Message_Constructor, Initialize) {
    Message message;
    EXPECT_EQ(1, 1);
}
