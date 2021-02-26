/*
 * Copyright (C) 2018 
 */

#ifndef COFFEE_H
#define COFFEE_H

#include <string>
#include <cctype>
#include "caffeine_beverage.h"

class Coffee : public Caffeine_beverage
{
	public:
		void brew() override
		{
			std::cout << "Dripping Coffee through filter\n";
		}

		void add_condiments() override
		{
			std::cout << "Adding Sugar and Milk\n\n";
		}

		bool customer_wants_condiments() override
		{
                        get_user_input();
			std::string val;

			for(auto c : answer_)
			{
				if(!islower(c))
					val.push_back(tolower(c));
				else
					val.push_back(c);
			}

			if(val == "y")
				return true;
			return false;
		}
	private:
		std::string get_user_input()
		{
			std::cout << "Would you like milk and sugar with your coffee\
				 (y/n)?\n";
			if(!(std::cin >> answer_))
				return "no";

			return answer_;
		}

		std::string answer_ {""};
};

class Tea : public Caffeine_beverage
{
	public:
		void brew() override
		{
			std::cout << "Dripping Tea through filter\n";
		}

		void add_condiments() override
		{
			std::cout << "Adding Lemon\n\n";
		}

		bool customer_wants_condiments() override
		{
                        get_user_input();
			std::string val;

			for(auto c : answer_)
			{
				if(!islower(c))
					val.push_back(tolower(c));
				else
					val.push_back(c);
			}

			if(val == "y")
				return true;
			return false;
		}
	private:
		std::string get_user_input()
		{
			std::cout << "Would you like milk and sugar with your coffee\
				 (y/n)?\n";
			if(!(std::cin >> answer_))
				return "no";

			return answer_;
		}

		std::string answer_ {""};
};

#endif
