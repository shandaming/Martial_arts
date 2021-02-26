/*
 * Copyright (C) 2018
 */

#ifndef DUCK_FACTORY_H
#define DUCK_FACTORY_H

#include <vector>
#include "quackable.h"

class Abstract_duck_factory
{
	public:
		virtual Quackable* create_mallard_duck() = 0;
		virtual Quackable* create_redhead_duck() = 0;
		virtual Quackable* create_duck_call() = 0;
		virtual Quackable* create_rubber_duck() = 0;
};

class Duck_factory : public Abstract_duck_factory
{
	public:
		Quackable* create_mallard_duck() override 
		{
			return new Mallard_duck(); 
		}

		Quackable* create_redhead_duck() override 
		{
			return new Redhead_duck(); 
		}

		Quackable* create_duck_call() override 
		{ 
			return new Duck_call(); 
		}

		Quackable* create_rubber_duck() override
		{
			return new Rubber_duck();
		}
};

class Counting_duck_factory : public Abstract_duck_factory
{
	public:
		Quackable* create_mallard_duck() override
		{
			return new Quack_counter(new Mallard_duck());
		}

		Quackable* create_redhead_duck() override
		{
			return new Quack_counter(new Redhead_duck());
		}

		Quackable* create_duck_call() override
		{
			return new Quack_counter(new Duck_call());
		}

		Quackable* create_rubber_duck() override
		{
			return new Quack_counter(new Rubber_duck());
		}
};

class Flock : public Quackable
{
	public:
		void add(Quack_observable* quacker) { list_.push_back(quacker); }

		void quack()
		{
			for(auto q : list_)
				q->quack();
		}

		void register_observer(Observer* observer) override {}

		void notify_observers() override {}
	private:
		std::vector<Quack_observable*> list_;
};

#endif
