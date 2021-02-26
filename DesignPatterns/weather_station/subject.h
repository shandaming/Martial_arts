/*
 * Copyright (C) 2018
 */

#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>
#include <algorithm>
#include <string>

class Subject;
class Observer
{
	public:
		Observer(Subject* s) : s_(s) {}
		virtual ~Observer() {}
		virtual void update(const std::string& s) = 0;
		virtual void display() = 0;
	protected:
		Subject* s_;
		std::string msg_;
};

class Subject
{
	public:
		void regist_observer(Observer* o)
		{
			observers_.push_back(o);
		}

		void remove_observer(Observer* o)
		{
			auto it = std::find(observers_.begin(), observers_.end(), o);
			if(it != observers_.end())
				observers_.erase(it);
		}
		void notify()
		{
			for(auto o : observers_)
				o->update("Display...");
		}
	private:
		std::vector<Observer*> observers_;
};



#endif
