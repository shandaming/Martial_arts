#include "config.h"

#include <iostream>

int main()
{
	std::string error;
	if(!CONFIG_MGR->load_initial("test.cfg", error))
		std::cout << error << std::endl;

	std::cout << "String = " << CONFIG_MGR->get_value_default("string", "abc", std::string("123")) <<std::endl;
	std::cout << "String = " << CONFIG_MGR->get_value_default("string", "abc", "123") <<std::endl;

	uint8_t z = 0;
	std::cout << "uint8 = " << CONFIG_MGR->get_value_default("int8", "int8", z) <<std::endl;

	uint16_t a = 0;
	std::cout << "uint16 = " << CONFIG_MGR->get_value_default("int16", "int16", a) <<std::endl;

	uint32_t b = 0;
	std::cout << "uint32 = " << CONFIG_MGR->get_value_default("int32", "int32", b) <<std::endl;

	uint64_t c = 0;
	std::cout << "uint64 = " << CONFIG_MGR->get_value_default("int64", "int64", c) <<std::endl;

	int8_t d = 0;
	std::cout << "int8 = " << CONFIG_MGR->get_value_default("int8", "uint8", d) <<std::endl;

	int16_t e = 0;
	std::cout << "int16 = " << CONFIG_MGR->get_value_default("int16", "uint16", e) <<std::endl;

	int32_t f = 0;
	std::cout << "int32 = " << CONFIG_MGR->get_value_default("int32", "uint32", f) <<std::endl;

	int64_t g = 0;
	std::cout << "int64 = " << CONFIG_MGR->get_value_default("int64", "uint64", g) <<std::endl;

	float h = 0.0f;
	std::cout << "float = " << CONFIG_MGR->get_value_default("double", "float", h) <<std::endl;

	double j = 0.0f;
	std::cout << "double = " << CONFIG_MGR->get_value_default("double", "double", j) <<std::endl;

	std::cout << "bool = " << CONFIG_MGR->get_value_default("boolean", "bool", true) <<std::endl;
	std::cout << "-------------------------------------------------------\n";
	CONFIG_MGR->print();
}
