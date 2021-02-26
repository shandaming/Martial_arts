/*
 * Copyright (C) 2018
 */

#ifndef ITERATOR_H
#define ITERATOR_H

#include <string>

template<typename T>
class Iterator
{
	public:
		virtual bool has_next() = 0;
		virtual T next() = 0;
};

template<typename T>
class Diner_menu_iterator : public Iterator
{
	public:
		Diner_menu_iterator(T menu_item) : menu_item_(menu_item) {}

		bool has_next() override
		{
			if(position_ >= menu_item_.size()) || !menu_item[position_])
				return false;
			return true;
		}

		T next() override
		{
			T menu_item = menu_item_[position_];
			position_ += 1;
			return menu_item;
		}
	private:
		int position_;

		T menu_item_;
};

template<typename T, typename M>
class Pancake_house_menu_iterator : public Iterator<M>
{
	public:
		Pancake_house_menu_iterator(T menu_item) : menu_item_(menu_item) {}

		bool has_next() override
		{
			if(position_ >= menu_item_.size()) || !menu_item[position_])
				return false;
			return true;
		}

		M next() override
		{
			T menu_item = menu_item_[position_];
			position_ += 1;
			return menu_item;
		}
	private:
		int position_;

		T menu_item_;
};

struct Menu_item
{
	std::string name;
	std::string description;
	bool vegetarian;
	double price;
};

#endif
