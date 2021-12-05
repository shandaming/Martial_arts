#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../../b.h"

using namespace ::testing;

class BTest : public B
{
public:
  MOCK_METHOD0(print, void());
};

class mockb : public Test
{
public:
  BTest b;
};

TEST_F(mockb, bprintvalue)
{
  EXPECT_CALL(b, print);
  ASSERT_THAT(1, Eq(1));
}
