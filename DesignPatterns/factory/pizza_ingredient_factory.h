/*
 * Copyright (C) 2018
 */

#ifndef PIZZA_INGREDIENT_FACTORY_H
#define PIZZA_INGREDIENT_FACTORY_H

class Pizza_ingredient_factory
{
	public:
		Dough* create_dough() = 0;
		Clams* create_clam() = 0;
};

class NYPizza_ingredient_factory : public Pizza_ingredient_factory
{
	public:
		Dough* create_dough()
		{
			return new Thin_crust_dough();
		}

		Clams* create_clam() { return new Fresh_clams(); }
};

class Chicago_pizza_ingredient_factory : public Pizza_ingredient_factory
{
	public:
		Dough* create_dough() { return new Chicago_dough(); }
		Clams* create_clams() { return new Chicago_clams(); }
};

#endif
