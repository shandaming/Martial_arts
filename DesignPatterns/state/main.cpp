/*
 * Copyright (C) 2018
 */

#include "gumball_machine.h"

int main()
{
	Gumball_machine* gm = new Gumball_machine(15);

	gm->insert_quarter();
	gm->eject_quarter();
	gm->turn_crank();
}
