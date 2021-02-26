/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef ACTIVE_OBJECT_ENQINE_H
#define ACTIVE_OBJECT_ENQINE_H

#include <vector>
#include "command.h"

class Active_object_enqine
{
	public:
		~Active_object_enqine() { clear(); }

		void add_command(Command& c)
		{
			its_commands.push_back(c);
		}

		void run()
		{
			while(!its_commands.empty())
			{
				Command c = its_commands.front();
				its_commands.erase(its_commands.begin());
				
				c.execute();
			}
		}
	private:
		void clear()
		{
			while(!its_commands.empty())
			{
				delete its_commands.fron();
				its_commands.erase(its_commands.begin());
			}
		}

		std::vector<Command*> its_commands;
};

#endif
