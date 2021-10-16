#include "test.h"
#include "address_v4.h"

TEST(addressV4, make)
{
	address_v4 v4;
	ASSERT_EQ(v4.to_bytes(), 0x00000000);
}

TEST(addressV4, assignent)
{
	address_v4, v;
	address_v4 v1(0xffffffff);
	ASSERT_TRUE(v != v1);
	v = v1;
	ASSERT_TRUE(v == v1);
}

TEST(AddressV4, compare)
{
	address_v4 v(0x0000000f);
	address_v4 v1(0x0000ffff);
	address_v4 v2(0x00ffffff);
	ASSERT_TRUE(v < v1);
	ASSERT_TRUE(v1 < v2);
	ASSERT_TRUE(v2 > v);
}
