/*
 * Copyright (C) 2018
 */

#ifndef GUMBALL_MACHINE_H
#define GUMBALL_MACHINE_H

#include "state.h"

class Gumball_machine
{
	public:
		Gumball_machine(int number_gumballs);

		void insert_quarter() { state_->insert_quarter(); }
		void eject_quarter() { state_->eject_quarter(); }

		void turn_crank() 
		{
			state_->turn_crank();
			state_->dispense();
		}

		void set_state(State* state) { state_ = state; }

		int get_count() const { return count_; }

		State* get_has_quarter_state() const { return has_quarter_state_; }
		State* get_no_quarter_state() const { return no_quarter_state_; }
		State* get_sold_out_state() const { return sold_out_state_; }
		State* get_sold_state() const { return sold_state_; }

		void release_ball()
		{
			std::cout << "A gumball comes rolling out the slot...\n";
			if(count_ != 0)
				count_ -= 1;
		}
	private:
		State* sold_out_state_;
		State* no_quarter_state_;
		State* has_quarter_state_;
		State* sold_state_;

		State* state_;
		int count_;
};

#endif
