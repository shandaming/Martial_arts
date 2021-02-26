/*
 * Copyright (C) 2018
 */

#ifndef CAFFEINE_BEVERAGE_H
#define CAFFEINE_BEVERAGE_H

#include <iostream>

class Caffeine_beverage
{
	public:
		virtual void prepare_recipe() final
		{
			boil_water();
			brew();
			pour_in_cup();

			if(customer_wants_condiments())
				add_condiments();
		}

		virtual void brew() = 0;

		virtual void add_condiments() = 0;

		void boil_water()
		{
			std::cout << "Boiling water\n";
		}

		void pour_in_cup()
		{
			std::cout << "Pouring into cup\n";
		}

		virtual bool customer_wants_condiments()
		{
			return true;
		}
};

#endif
