/*
 * Copyright (C) 2018
 */

#ifndef DUCK_H
#define DUCK_H

#include <iostream>

class Duck
{
	public:
		virtual void quack() = 0;
		virtual void fly() = 0;
};

class Mallard_duck : public Duck
{
	public:
		void quack() override
		{
			std::cout << "Quack\n";
		}

		void fly() override
		{
			std::cout << "I'm flying\n";
		}
};

#endif
