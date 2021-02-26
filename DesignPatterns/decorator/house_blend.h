/*
 * Copyright (C) 2018
 */

#ifndef HOUSE_BLEND_H
#define HOUSE_BLEND_H

#include "beverage.h"

class House_blend : public Beverage
{
	public:
		House_blend() : Beverage() { description_ = "House Blend Coffee"; }

		double cost() const override { return 0.89; }
};

#endif
