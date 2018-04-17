//
// Created by liao xiangsen on 2018/4/18.
//

#include "parser.h"
#include "gtest/gtest.h"

TEST(JUST_TEST, FIRST) {
  EXPECT_EQ(5, monitor::addTest(2, 3));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

