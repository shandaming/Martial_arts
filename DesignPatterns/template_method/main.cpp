/*
 * Copyright (C) 2018
 */

#include "coffee.h"

int main()
{
	Coffee* c = new Coffee();
	Tea* t = new Tea();

	std::cout << "Making tea...\n";
	t->prepare_recipe();

	std::cout << "Making coffee...\n";
	c->prepare_recipe();
}
