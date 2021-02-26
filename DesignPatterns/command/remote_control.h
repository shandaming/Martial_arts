/*
 * Copyright (C) 2018
 */

#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include <string>

class Remote_control
{
	public:
		void set_command(int slot, Command* on_command, 
				command* off_command)
		{
			on_commands_[slot] = on_command;
			off_commands_[slot] = off_command;
		}

		void on_button_was_pushed(int slot)
		{
			if(on_commands_[slot])
				on_commands_[slot]->excute();
			undo_command_ = on_commands_[slot];
		}

		void off_button_was_pushed(int slot)
		{
			if(off_commands_[slot])
				off_commands_[slot]->excute();
			undo_command_ = off_commands_[slot];
		}

		void undo_button()
		{
			undo_command_->undo();
		}

		void to_string() const
		{
			std::string buff = "\n------ Remote Control ------\n";
			for(int i = 0; i < 7; ++i)
			{
				buff + "[slot " + std::to_string(i) + "] " + 
					std::string(typeid(*on_commands_[i]).name()) + "\n";
			}

			std::cout << buff;
		}
	private:
		Command* on_commands_[7] = nullptr;
		Command* off_commands_[7] = nullptr;

		Command* undo_command_;
};

#endif
