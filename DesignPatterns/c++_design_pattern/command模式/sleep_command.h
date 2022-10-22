/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef SLEEP_COMMAND_H
#define SLEEP_COMMAND_H

#include <ctime>
#include "command.h"
#include "active_object_enqine.h"

class Sleep_command : public Command
{
	public:
		Sleep_command(long milliseconds, Active_object_enqine* e, 
				Command* wakeup_command) : milliseconds_(0), 
											enqine_(nullptr),
											wakeup_command_(nullptr)
		{
			sleep_command(milliseconds, e, wakeup_command);
		}

		virtual ~Sleep_command()
		{
			if(enqine_)
				delete enqine_;
			if(wakeup_command_)
				delete wakeup_command_;
		}

		void sleep_command(long milliseconds, Active_object_enqine* e,
				Command* wakeup_command)
		{
			sleep_time_ = milliseconds;
			enqine_ = e;
			wakeup_command_ = wakeup_command
		}

		void execute() override
		{
			// long current_time = NOW();
			if(!started_)
			{
				started_ = true;
				start_time = curren_time;
				enqune.add_command(this);
			}
			else if((current_time - start_time_) < sleep_time_)
				enqine.add_command(this);
			else
				enqine.add_command(wakeup_command);
		}
	private:
		Command* wakeup_command_;
		Active_object_enqine* enqine_;
		long sleep_time_;
		long start_time_;
		bool started_;
};

class Test_sleep_command
{
	public:
		void test_sleep()
		{
			Command* wakeup = new Command();
			assert(wakeup);

			Active_object_enqine* e = new Active_object_enqine();
			assert(e);

			Sleep_command* c = new Sleep_command(1000, e, wakeup);
			e.add_command(c);
			// long start = NOW();
			e.run();
			// long stop = NOW();
			// long sleep_time = stop - start;
			assert("Sleep_time " + sleep_time + " expected > 1000", 
					sleep_time > 1000);
			assert("sleep_time " + sleep_time + " expected < 1000",
					sleep_time < 1100);
			assert("Command Executed", command_executed);
		}
	private:
		bool command_executed = false;
};

#endif
