/*
 * Copyright (C) 2018
 */

#ifndef CONDIMENT_DESCORATOR_H
#define CONDIMENT_DESCORATOR_H

#include "beverage.h"

class Condiment_decorator : public Beverage
{
	public:
		Condiment_decorator() : Beverage() {}
		virtual std::string get_description() const override {}
                virtual double cost() const override {}
};

class Mocha : public Condiment_decorator
{
	public:
		Mocha(Beverage* b) : Condiment_decorator(), beverage_(b) {}

		std::string get_description() const override
		{
			return beverage_->get_description() + ", Mocha";
		}

		double cost() const override { return beverage_->cost() + 0.20; }
	private:
		Beverage* beverage_;
};

class Soy : public Condiment_decorator
{
	public:
		Soy(Beverage* b) : Condiment_decorator(), beverage_(b) {}
		std::string get_description() const override
		{
			return beverage_->get_description() + ", Soy";
		}

		double cost() const override { return beverage_->cost() + 0.15; }
	private:
		Beverage* beverage_;
};

class Whip : public Condiment_decorator
{
	public:
		Whip(Beverage* b) : Condiment_decorator(), beverage_(b) {}

		std::string get_description() const override
		{
			return beverage_->get_description() + ", Whip";
		}

		double cost() const override { return  beverage_->cost() + 1.0; }
	private:
		Beverage* beverage_;
};

#endif
