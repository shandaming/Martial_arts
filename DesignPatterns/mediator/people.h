/*
 * Copyright (C) 2018
 */

#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <iostream>

class Mediator;

class People
{
	public:
		People(Mediator* m) : mediator_(m) {}

		virtual void sent_msg(const std::string& msg) = 0;
		virtual void receive_msg(const std::string& msg) = 0;
	protected:
		Mediator* mediator_;
};

class Mediator
{
	public:
                void set_people(People* p1, People* p2) { landlord_ = p1, renter_ = p2; }

		virtual void sent_landlord_msg(const std::string& msg) = 0;
		virtual void sent_renter_msg(const std::string& msg) = 0;
	protected:
		People* landlord_;
		People* renter_;
};

class Renter : public People
{
	public:
		Renter(Mediator* m) : People(m) {}

		void sent_msg(const std::string& msg) override
		{
			mediator_->sent_landlord_msg(msg);
		}

		void receive_msg(const std::string& msg) override
		{
			std::cout << "租房人收到信息: " << msg << "\n";
		}
};

class Landlord : public People
{
	public:
		Landlord(Mediator* m) : People(m) {}

		void sent_msg(const std::string& msg) override
		{
			mediator_->sent_renter_msg(msg);
		}

		void receive_msg(const std::string& msg) override
		{
			std::cout << "房东收到信息: " << msg << "\n";
		}
};

class House_mediator : public Mediator
{
	public:
		void sent_landlord_msg(const std::string& msg) override 
		{ 
			landlord_->receive_msg(msg);
		}

		void sent_renter_msg(const std::string& msg) override 
		{ 
			renter_->receive_msg(msg);
		}
};

#endif
