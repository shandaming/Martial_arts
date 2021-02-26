/*
 * Copyright (C) 2018
 */

#ifndef PIZZA_STORE_H
#define PIZZA_STORE_H

#include <string>
#include "pizza.h"

class Pizza_store
{
	public:
		virtual ~Pizza_store() {}

		virtual Pizza* create_pizza(const std::string& type) = 0;

		Pizza* order_pizza(const std::string& type)
		{
			Pizza* pizza = create_pizza(type);

			pizza->prepare();
			pizza->bake();
			pizza->cut();
			pizza->box();

			return pizza;
		}
};

#endif
