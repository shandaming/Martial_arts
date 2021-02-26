/*
 * Copyright (C) 2018
 */

#ifndef OBSERVER_H
#define OBSERVER_H

#include <iostream>

#include "subject.h"

class Display1 : public Observer
{
	public:
		Display1(Subject* s) : Observer(s)
		{
			s_->regist_observer(this);
		}

		void update(const std::string& s) override
		{
			msg_ = s;
			display();
		}

		void display() override
		{
			std::cout << msg_ << "\n";
		}
};

class Display2 : public Observer
{
	public:
		Display2(Subject* s) : Observer(s)
		{
			s_->regist_observer(this);
		}

		void update(const std::string& s) override
		{
			msg_ = s;
			display();
		}

		void display() override
		{
			std::cout << msg_ << "\n";
		}
};

#endif
