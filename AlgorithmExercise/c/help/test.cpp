#include "type_list.h"
#include <iostream>

struct base {};
struct derived : base{};

int main()
{
	//std::cout << "char double = " << IS_EQUAL(INT(5), FALSE())::value <<
		//". double double = " << IS_EQUAL(TRUE(), BOOL(true))::value << std::endl;
	std::cout << (VALUE(INT(5)) + VALUE(INT(6))) << std::endl;

	std::cout << " char int = " << VALUE(IS_CONVERTIBLE(char, int)) << std::endl;

	std::cout << " base derived = " << VALUE(IS_CONVERTIBLE(base*, derived*)) << std::endl;
	std::cout << " derived base = " << VALUE(IS_CONVERTIBLE(derived*, base*)) << std::endl;

	std::cout << " derived base = " << VALUE(IS_BASE_OF(derived, base)) << std::endl;
	std::cout << " derived base = " << VALUE(IS_BASE_OF(base, derived)) << std::endl;

	sum<INT(1), INT(2)>::result b;
	SUM(INT(1), INT(2), INT(3), INT(4), INT(5), INT(6), INT(7), INT(8), INT(9)) a;
	std::cout << " 1-9 = " << VALUE(SUM(INT(1), INT(2), INT(3), INT(4), INT(5), INT(6), INT(7), INT(8), INT(9))) << std::endl;

	ASSERT_TRUE(BOOLEAN(true));
	ASSERT_TRUE(NOT(FALSE()));
	//ASSERT_FALSE(OR(TRUE(), FALSE()));

	ASSERT_EQUAL(INT(0), INT(0));
	ASSERT_UNEQUAL(INT(0), INT(1));
	ASSERT_EQUAL(IF(TRUE(), int, char), int);
	ASSERT_EQUAL(IF(FALSE(), int, char), char);

	type_list<char, short, int, long, float, double>::result list;
	static_assert(VALUE(is_equal<typename value_list<0, 1, 2>::result, typename type_list<INT(0), INT(1), INT(2)>::result>::result), "Assert Failed. expect  be equal to !");
	//std::cout << "type_list length = " << VALUE(length<type_list<char, int, short>>) << std::endl;
	//static_assert(VALUE(is_equal<length<type_list<char, int, short>::result>, INT(3)>::result), "Assert Failed. expect  be equal to !");
}
