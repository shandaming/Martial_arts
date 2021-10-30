/*
 * Copyright (C) 2021
 */

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "socket_operations.h"

using namespace ::testing;

class socket_operation
{
public:
	MOCK_METHOD0(inet_ntop, const char*());
};

static socket_operation socket_op;
const char* inet_ntop(int domain, const void* addrptr, char* dst_str, size_t len, uint64_t scope_id, std::error_code& ec)
{
	return socket_op.inet_ntop();
}

TEST(asocket, test_inet_ntop)
{
	EXPECT_CALL(socket_op, inet_ntop).WillRepeatedly(Return("123"));

	std::error_code ec;
	ASSERT_THAT(inet_ntop(1, IsNull(), IsNull(), 1, 1, ec), Eq("123"));
}
