/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef COMMAND_H
#define COMMAND_H

class Command
{
	public:
		virtual ~Command();
		virtual void execute();
};

#endif
