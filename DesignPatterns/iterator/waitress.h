/*
 * Copyright (C) 2018
 */

#ifndef WAITRESS_H
#define WAITRESS_H

class Waitress
{
	public:
		Waitress();

		void print_menu()
		{
			Iterator<std::list<Menu_item*>> pancake_iterator =
				Pancake_house_menu->create_iterator();
			Iterator<Menu_item*> diner_iterator = 
				Diner_menu->create_iterator();

			std::cout << "MENU\n----\nBREAKFAST";
			print_menu(pancake_iterator);

			std::cout << "\nLUNCH";
			print_menu(diner_iterator);
		}
	private:
		template<typename T>
		void print_menu(Iterator<T> iterator)
		{
			while(iterator->has_next())
			{
				T menu_item = iterator->next();
				std::cout << menu_item->name << ", "
					<< menu_item->price << " -- "
					<< menu_item->description << "\n";
			}
		}

		Pancake_house_menu* pancake_house_menu_;
		Diner_menu* diner_menu_;
};

#endif
