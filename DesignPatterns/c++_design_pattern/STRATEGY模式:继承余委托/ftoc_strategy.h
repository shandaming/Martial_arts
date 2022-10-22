/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FTOC_STRATEGY_H
#define FTOC_STRATEGY_H

#include <iostream>
#include "application.h"

class Ftoc_strategy : public Application
{
	public:
		void init() override
		{
			val_ = 0;
			is_done_ = false;
		}

		void idle() override
		{
			if(!(std::cin >> val_))
				is_done_ = true;
			else
			{
				double celcius = 5.0 / 9.0 * (val_ - 32);
				std::cout << celcius << "\n";
			}

		}

		void cleanup() override
		{
			std::cout << "ftoc exit" << "\n";
		}

		bool done() override { return is_done_; }
	private:
		double val_;
		bool is_done_;
};

#endif
