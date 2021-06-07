/*
 * Copyright (C) 2020
 */

#include <gtest/gtest.h>
/*
致命断言	        非致命断言	        验证
ASSERT_EQ(val1, val2);	EXPECT_EQ(val1, val2);	val1 == val2
ASSERT_NE(val1, val2);	EXPECT_NE(val1, val2);	val1 != val2
ASSERT_LT(val1, val2);	EXPECT_LT(val1, val2);	val1 < val2
ASSERT_LE(val1, val2);	EXPECT_LE(val1, val2);	val1 <= val2
ASSERT_GT(val1, val2);	EXPECT_GT(val1, val2);	val1 > val2
ASSERT_GE(val1, val2);	EXPECT_GE(val1, val2);	val1 >= val2

致命断言	非致命断言	验证
ASSERT_STREQ(str1,str2);	EXPECT_STREQ(str1,str2);	这两个C字符串具有相同的内容
ASSERT_STRNE(str1,str2);	EXPECT_STRNE(str1,str2);	这两个C字符串的内容不同
ASSERT_STRCASEEQ(str1,str2);	EXPECT_STRCASEEQ(str1,str2);	两个C字符串的内容相同，忽略大小写
ASSERT_STRCASENE(str1,str2);	EXPECT_STRCASENE(str1,str2);	两个C字符串的内容不同，忽略大小写
*/

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
