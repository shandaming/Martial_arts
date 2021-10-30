#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "address_v4.h"

using namespace ::testing;

class aaddress_v4 : public Test
{
public:
	address_v4 av;
};

TEST_F(aaddress_v4, make_a_loopback)
{
	av = make_address_v4("127.0.0.1");

	ASSERT_TRUE(av.is_loopback());
}
