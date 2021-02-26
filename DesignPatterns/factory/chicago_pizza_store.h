/*
 * Copyright (C) 2018
 */

#ifndef CHICAGO_PIZZA_STORE_H
#define CHICAGO_PIZZA_STORE_H

#include "pizza_store.h"

class Chicago_pizza_store : public Pizza_store
{
	public:
		Pizza* create_pizza(const std::string& type)
		{
			if(type == "cheese")
				return new Chicago_style_cheese_pizza();
			/*
			else if(type == "veggie")
				return new Chicago_style_veggie_pizza();
			else if(type == "clam")
				return new Chicago_style_clam_pizza();
			else if(type == "pepperoni")
				return new Chicago_style_pepperoni_pizza();
			*/
			else
				return nullptr;
		}
};

#endif
