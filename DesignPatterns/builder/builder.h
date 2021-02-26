/*
 * Copyright (C) 2018
 */

#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <iostream>
#include <vector>

class Builder
{
	public:
		virtual void build_day(const std::string& date) = 0;
		virtual void add_hotel(const std::string& name) = 0;
		virtual void add_reservation() = 0;
		virtual void add_special_event(const std::string& event) = 0;
		virtual void add_tickets(const std::string& name) = 0;
		virtual std::vector<std::string> get_vacation_planner() const = 0;
                virtual void clear() = 0;
};

class Vacation_builder : public Builder
{
	public:
		void build_day(const std::string& date) override 
		{
			vacation_planner_.push_back(date);
		}

		void add_hotel(const std::string& name) override
		{
			vacation_planner_.push_back(name);
		}

		void add_reservation() override {}

		void add_special_event(const std::string& event) override
		{
			vacation_planner_.push_back(event);
		}

		void add_tickets(const std::string& name) override
		{
			vacation_planner_.push_back(name);
		}

		std::vector<std::string> get_vacation_planner() const override 
		{
			return vacation_planner_;
		}

		void clear() override { vacation_planner_.clear(); }
	private:
		std::vector<std::string> vacation_planner_;
};

class Client
{
	public:
		Client() : builder_(new Vacation_builder())
		{
			builder_->build_day("2018-01-23");
			builder_->add_hotel("zhufang1");
			builder_->add_tickets("menpian1");

			auto planner = builder_->get_vacation_planner();

			for(auto s : planner)
				std::cout << s << " ";
			std::cout << "\n";
        
                        builder_->clear();
			builder_->build_day("2018-09-23");
			builder_->add_hotel("zhufang2");
			builder_->add_tickets("menpian2");

			auto planner1 = builder_->get_vacation_planner();

			for(auto s : planner1)
				std::cout << s << " ";
			std::cout << std::endl;
		}
	private:
		Builder* builder_;
};

#endif
