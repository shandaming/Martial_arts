/*
 * Copyright (C) 2018
 */

#ifndef STATE_H
#define STATE_H

#include <iostream>

class State
{
	public:
		virtual void insert_quarter() = 0;
		virtual void eject_quarter() = 0;
		virtual void turn_crank() = 0;
		virtual void dispense() = 0;
};

class Gumball_machine;
class No_quarter_state;
class Has_quarter_state;
class Slod_state;
class Sold_out_state;



class No_quarter_state : public State
{
	public:
		No_quarter_state(Gumball_machine* g) : gumball_machine_(g) {}

		void insert_quarter() override;

		void eject_quarter() override
		{
			std::cout << "You haven't inserted a quarter\n";
		}

		void turn_crank() override
		{
			std::cout << "You turned, but there's no quarter\n";
		}

		void dispense() override
		{
			std::cout << "You need to pay first\n";
		}
	private:
		Gumball_machine* gumball_machine_;
};

class Has_quarter_state : public State
{
	public:
		Has_quarter_state(Gumball_machine* g) : gumball_machine_(g) {}

		void insert_quarter() override
		{
			std::cout << "You can't inserted another quarter\n";
		}

		void eject_quarter() override;

		void turn_crank() override;

		void dispense() override
		{
			std::cout << "No gumball dispensed\n";
		}
	private:
		Gumball_machine* gumball_machine_;
};

class Sold_state : public State
{
	public:
		Sold_state(Gumball_machine* g) : gumball_machine_(g) {}

		void insert_quarter() override
		{
			std::cout << "Please wait, we're already giving you a \
				gumball\n";
		}

		void eject_quarter() override
		{
			std::cout << "Sorry, you already turned the crank\n";
		}

		void turn_crank() override
		{
			std::cout << "Turning twice doesn't get you another gumball\n";
		}

		void dispense() override;
	private:
		Gumball_machine* gumball_machine_;
};

class Sold_out_state : public State
{
	public:
		Sold_out_state(Gumball_machine* g) : gumball_machine_(g) {}

		void insert_quarter() override
		{
			std::cout << "You can't insert a quarter, the machine is sold\
				out\n";
		}

		void eject_quarter() override
		{
			std::cout << "You can't eject, you haven't inserted a quarter\
				yet\n";
		}

		void turn_crank() override
		{
			std::cout << "You turned, but there are no gumballs\n";
		}

		void dispense() override
		{
			std::cout << "No gumball dispensed\n";
		}
	private:
		Gumball_machine* gumball_machine_;
};



#endif
