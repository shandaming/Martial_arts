/*
 * Copyright (C) 2018
 */

#ifndef ESPRESSO_H
#define ESPRESSO_H

#include "beverage.h"

class Espresso : public Beverage
{
	public:
		Espresso() : Beverage() { description_ = "Espresso"; }

		double cost() const override { return 1.99; }
};

#endif
