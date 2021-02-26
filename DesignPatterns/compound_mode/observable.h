/*
 * Copyright (C) 2018
 */

#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <vector>
#include <iostream>

class Quack_observable;

class Observer
{
	public:
		virtual void update(Quack_observable* duck) = 0;
};

class Quack_observable
{
	public:
                virtual void quack() = 0;
		virtual void register_observer(Observer* observer) = 0;
		virtual void notify_observers() = 0;
};

class Quackologist : public Observer
{
	public:
		void update(Quack_observable* duck) override
		{
			std::cout << "I'm Observer\n";
		}
};

class Observable : public Quack_observable
{
	public:
		Observable(Quack_observable* duck) : duck_(duck) {}

                void quack() override {}

		void register_observer(Observer* observer) override
		{
			list_.push_back(observer);
		}

		void notify_observers() override
		{
			for(auto o : list_)
				o->update(duck_);
		}
	private:
		std::vector<Observer*> list_;

		Quack_observable* duck_;
};

#endif
