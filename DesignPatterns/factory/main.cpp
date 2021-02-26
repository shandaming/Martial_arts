/*
 * Copyright (C) 2018
 */

#include "chicago_pizza_store.h"
#include "nypizza_store.h"

int main()
{
	NYPizza_store* nystore = new NYPizza_store();
	Chicago_pizza_store* chstore = new Chicago_pizza_store();

	Pizza* p1 = nystore->order_pizza("cheese");
	std::cout << "Ethan ordered a " << p1->get_name() << "\n";

	Pizza* p2 = chstore->order_pizza("cheese");
	std::cout << "Joel ordered a " << p2->get_name() << "\n";
}
