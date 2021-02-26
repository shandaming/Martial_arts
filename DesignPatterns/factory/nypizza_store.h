/*
 * Copyright (C) 2918
 */

#ifndef nypizza_store_h
#define nypizza_store_h

#include "pizza_store.h"

class NYPizza_store : public Pizza_store
{
	public:
		Pizza* create_pizza(const std::string& type)
		{
			/*
			 * Pizza_ingredient_factory* ft = new NYPizza_ingredient_factory();
			 *
			 */
			if(type == "cheese")
				return new NYStyle_cheese_pizza(/* ft */);
			/*
			else if(type == "veggie")
				return new NUStyle_veggie_pizza();
			else if(type == "clam")
				return new NYStyle_clam_pizza();
			else if(type == "pepperoni")
				return new NYStyle_pepperoni_pizza();
			*/
			else
				return nullptr;
		}
};

#endif
