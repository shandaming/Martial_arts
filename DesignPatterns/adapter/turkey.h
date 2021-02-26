/*
 * Copyright (C) 2018
 */

#ifndef TRUKEY_H
#define TRUKEY_H

#include <iostream>

class Turkey
{
	public:
		virtual void gobble() = 0;
		virtual void fly() = 0;
};

class Wild_turkey : public Turkey
{
	public:
		void gobble() override
		{
			std::cout << "Gobble gobble\n";
		}

		void fly() override
		{
			std::cout << "I'm flying a short distance\n";
		}
};

#endif
