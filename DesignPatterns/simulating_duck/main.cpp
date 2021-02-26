/*
 * Copyright (C) 2018 by Shan Daming
 */

#include "duck.h"

int main()
{
	Duck* d = new Model_duck();
	d->fly();
	d->set_fly(new Fly_rocket_powered);
	d->fly();

	return 0;
}
