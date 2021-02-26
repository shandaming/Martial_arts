/*
 * Copyright (C) 2018
 */

#ifndef PIZZA_H
#define PIZZA_H

#include <string>
#include <iostream>
#include <vector>

class Pizza
{
	public:
		virtual ~Pizza() {}

		void prepare() // 抽象工厂要定义为纯虚函数
		{
			std::cout << "Preparing " << name_ << "\n";
			std::cout << "Tossing dough...\n";
			std::cout << "Adding sauce...\n";
			std::cout << "Adding toppings: \n";

			for(int i = 0; i < toppings_.size(); ++i)
				std::cout << toppings_[i] << "\n";

		}

		virtual void bake()
		{
			std::cout << "Bake for 25 minutes at 250\n";
		}

		virtual void cut()
		{
			std::cout << "Cutting the pizza into diagonal slices\n";
		}

		virtual void box()
		{
			std::cout << "Place pizza in official Pizza Store box\n";
		}

		std::string get_name() const  { return name_; };
	protected:
		std::string name_;
		std::string dough_;
		std::string sauce_;

		std::vector<std::string> toppings_;

		/*
		 * 抽象工厂
		 * Dough* dough_;
		 * Clams* clams_;
		 */
};

class NYStyle_cheese_pizza : public Pizza
{
	public:
		NYStyle_cheese_pizza(/* Pizza_ingredient_factory* ft*/)
		{
			name_ = "NY Style Sauce and Cheese Pizza";
			dough_ = "Thin Crust Dough";
			sauce_ = "Marinara Sauce";

			toppings_.push_back("Grated Reggiano Cheese");

			/*
			 * dough_ = ft->create_dough();
			 * climas_ = ft->create_Climas();
			 */
		}
};

class Chicago_style_cheese_pizza : public Pizza
{
	public:
		Chicago_style_cheese_pizza()
		{
			name_ = "Chicage Style Deep Dish Cheese Pizza";
			dough_ = "Extra Thick Crust Dough";
			sauce_ = "Plum Tomato Sauce";

			toppings_.push_back("Shredded Mozzarella Cheese");
		}

		void cut() override
		{
			std::cout << "Cutting the pizza into square slices\n";
		}
};

#endif
