/*
 * Copyright (C) 2018
 */

#ifndef QUACK_COUNTER_H
#define QUACK_COUNTER_H

#include "quackable.h"

class Quack_counter : public Quackable
{
	public:
		Quack_counter(Quackable* d) : duck_(d) {}

		void quack()
		{
			duck_->quack();
			++number_quacks_;
		}

		void register_observer(Observer* observer) override {}

		void notify_observers() override {}

		static int get_quacks() { return number_quacks_; }
	private:
		Quackable* duck_;

		static int number_quacks_;
};

#endif
