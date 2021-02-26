/*
 * Copyright (C) 2020
 */

#include "test.h"
#include "socket.h"


TEST(socket, make)
{
	socket sock;
	ASSERT_FALSE(sock.is_open());
	sock.open(6);
	ASSERT_TRUE(sock.is_open());

	socket sock1(6);
	ASSERT_TRUE(sock1.is_open());
}

TEST(socket, close)
{
	socket sock(5);
	sock.close();
	ASSERT_FALSE(sock.is_open());

	sock.open(6);
	sock.close();
	ASSERT_FALSE(sock.is_open());
}

TEST(socket, assignment)
{
	socket sock1;
	socket sock2(5);
	ASSERT_TRUE(sock1 != sock2);
	sock1 = sock2;
	ASSERT_TRUE(sock1 == sock2);
}

TEST(socke, move)
{
	socket sock1;
	socket sock2(6);
	sock1 = std::move(sock2);
	ASSERT_TRUE(sock1.is_open());
	ASSERT_FALSE(sock2.is_open());
}
