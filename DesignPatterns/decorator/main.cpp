/*
 * Copyright (C) 2018
 */

#include <iostream>
#include "espresso.h"
#include "house_blend.h"
#include "condiment_decorator.h"

int main()
{
	Beverage* beverage = new Espresso();
	std::cout << beverage->get_description() << " $" << beverage->cost() 
		<< "\n";

	beverage = new Mocha(beverage);
	beverage = new Soy(beverage);
	beverage = new Whip(beverage);
	std::cout << beverage->get_description() << " $" << beverage->cost() 
		<< "\n";

	Beverage* b = new House_blend();
	b = new Mocha(b);
	b = new Soy(b);
	b = new Whip(b);
	std::cout << b->get_description() << " $" << b->cost() << "\n";

	return 0;
}
