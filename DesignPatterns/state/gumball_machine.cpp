/*
 * Copyright (C) 2018
 */

#include "gumball_machine.h"

Gumball_machine::Gumball_machine(int number_gumballs) :
                        sold_out_state_(new Sold_out_state(this)),
			no_quarter_state_(new No_quarter_state(this)),
			has_quarter_state_(new Has_quarter_state(this)),
			sold_state_(new Sold_state(this)),
			state_(sold_out_state_),
			count_(number_gumballs)
{
        if(number_gumballs > 0)
	        state_ = no_quarter_state_;
}
