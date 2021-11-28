/*
 * Copyright (C) 2021
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <window.h>

using namespace ::testing;

class mock_window : public window
{
public:
	MOCK_METHOD()

	window* win;
};

TEST(mock_window, create_a_window)
{
	window win("123", )
}
