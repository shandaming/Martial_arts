/*
 * Copyright (C) 2018 
 */

#ifndef BEVERAGE_H
#define BEVERAGE_H

#include <string>

class Beverage
{
	public:
		Beverage() : description_("unknow Beverage") {}
		virtual ~Beverage() {}
		virtual std::string get_description() const { return description_; }
		virtual double cost() const = 0;
	protected:
		std::string description_;
};

#endif
