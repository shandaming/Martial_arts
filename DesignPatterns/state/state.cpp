/*
 * Copyright (C) 2018
 */

#include "state.h"
#include "gumball_machine.h"

		void No_quarter_state::insert_quarter()
		{
			std::cout << "You inserted a quarter\n";
			gumball_machine_->set_state(
					gumball_machine_->get_has_quarter_state());
		}

		void Has_quarter_state::eject_quarter()
		{
			std::cout << "Quarter returned\n";
			gumball_machine_->set_state(
					gumball_machine_->get_no_quarter_state());
		}

		void Has_quarter_state::turn_crank()
		{
			std::cout << "You turned\n";
			gumball_machine_->set_state(
					gumball_machine_->get_sold_state());
		}

		void Sold_state::dispense()
		{
			gumball_machine_->release_ball();
			if(gumball_machine_->get_count() > 0)
				gumball_machine_->set_state(
						gumball_machine_->get_no_quarter_state());
            else
            {
				std::cout << "Oops, out of gumballs!\n";
                gumball_machine_->set_state(
						gumball_machine_->get_sold_out_state());
            }
		}
