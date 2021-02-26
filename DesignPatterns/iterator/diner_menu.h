/*
 * Copyright (C) 2018
 */

#ifndef DINER_MENU_H
#define DINER_MENU_H

#include "iterator.h"

class Diner_menu
{
	public:
		Diner_menu()
		{
			menu_items_ = new Menu_item[6];

			add_item("Vegetarian BLT", "Bacon with lettuce", true, 2.99);
			add_item("BLT", "Tomato on whole wheat", false, 2.99);
		}

		//Menu_item* get_menu_items() const { return menu_items_; }

		Iterator<Menu_item*> create_iterator()
		{
			return new Diner_menu_iterator<Menu_item,Menu_item*>(menu_items_);
		}

		void add_item(const char* name, const char* description, 
				bool vegetarian, double price)
		{
			Menu_item* item = new Menu_item();
			assert(item);
			item->name = name;
			item->description = description;
			item->vegetarian = vegetarian;
			item->price = price;

			if(number_items_ < max_items_)
			{
				menu_items_[number_items_] = item;
				number_items_ += 1;
			}
		}

		int size() const { return max_items_; }
	private:
		int max_items_;
		int number_items_;
		Menu_item* menu_items_;
};

#endif
