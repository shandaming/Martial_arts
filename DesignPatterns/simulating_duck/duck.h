/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef DUCK_H
#define DUCK_H

#include <iostream>

class Fly
{
	public:
		virtual void fly() = 0;
};

class Fly_no_way : public Fly
{
	public:
		void fly() override
		{
			std::cout << "Fly No Way\n";
		}
};

class Fly_rocket_powered : public Fly
{
	public:
		void fly() override
		{
			std::cout << "fly rocket powered\n";
		}
};

class Duck
{
	public:
		Duck() : fly_(0) {}
		virtual ~Duck() { if(fly_) delete fly_; }

		virtual void swim() {}
		virtual void display() {}

		void set_fly(Fly* f) { fly_ = f;}

		virtual void fly() = 0;
	protected:
		Fly* fly_;
};

class Model_duck : public Duck
{
	public:
		Model_duck() : Duck()
		{
			fly_ = new Fly_no_way();
		}

		void fly() override
		{
			fly_->fly();
		}
};

#endif
