/*
 * Copyright (C) 2018
 */

#ifndef PANCAKE_HOUSE_MENU_H
#define PANCAKE_HOUSE_MENU_H

#include <list>
#include "iterator.h"

class Pancake_house_menu
{
	public:
		Pancake_house_menu()
		{
			add_item("Pancake Breakfast", "Pancakes with scrambled eggs",
					true, 2.99);
			add_item("Regular", "Pancakes with fried eggs, sausage",
					false, 2.99);
		}

		Iterator<std::vector<Menu_item*>> create_iterator()
		{
			return new Pancake_house_menu_iterator<
				std::vector<Menu_item*>, Menu_item*>(menu_items_);
		}

		void add_item(const std::string& name, 
				const std::string& description,
				bool vegetarian, 
				double price)
		{
			Menu_item* item = new Menu_item();
			assert(item);
			item->name = name;
			item->description = description;
			item->vegetarian = vegetarian;
			item->price = price;

			menu_items_.push_back(item);
		}
	private:
		std::list<Menu_item*> menu_items_;
};

#endif
