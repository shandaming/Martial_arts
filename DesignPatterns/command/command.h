/*
 * Copyright (C) 2018
 */

#ifndef COMMAND_H
#define COMMAND_H

class Command
{
	public:
		virtual void excute() = 0;
		virtual void undo() = 0;
};

class Light_on_command : public Command
{
	public:
		Light_on_command(Light* light) : light_(light) {}

		void execute()
		{
			light_->on();
		}

		void undo()
		{
			light_->off();
		}
	private:
		Light* light_;
};

class Light_off_command : public Command
{
	public:
		Light_off_command(Light* light) : light_(light) {}

		void excute()
		{
			light_->off();
		}

		void undo()
		{
			light_->on();
		}
	private:
		Light* light_;
};

class Stereo_on_CD_command : public Command
{
	public:
		Stereo_on_CD_command(Stereo* s) : stereo_(s) {}

		void excute()
		{
			stereo_->on();
			stereo_->set_cd();
			stereo_->set_volume(11);
		}
	private:
		Stereo* stereo_;
};

class No_command : public Command
{
	public:
		void excute() {}
};

#endif
