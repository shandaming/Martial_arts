/*
 * Copyright (C) 2018
 */

#ifndef TURKEY_ADAPTER_H
#define TURKEY_ADAPTER_H

#include "duck.h"
#include "turkey.h"

class Turkey_adapter : public Duck
{
	public:
		Turkey_adapter(Turkey* t) : turkey_(t) {}

		void quack() override
		{
			turkey_->gobble();
		}

		void fly() override
		{
			for(int i = 0; i < 5; ++i)
				turkey_->fly();
		}
	private:
		Turkey* turkey_;
};

#endif
