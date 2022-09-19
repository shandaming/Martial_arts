/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef DELAYED_TYPER_H
#define DELAYED_TYPER_H

class Delayed_typer : public Command
{
	public:
		Delayed_typer(long delay, char c)
		{
			its_delay = delay;
			its_char = c;
		}

		void excute() override
		{
			printf("%c\n", its_char);
			if(!stop)
				delay_and_repeat();
		}
	private:
		void delay_and_repeat()
		{
			enqine.add_command(new Sleep_command(its_delay, enqine_, this);)
		}

		long its_delay_;
		char its_char_;
		
		static Active_object_enqine* enqine;
		static bool stop;
};

#endif
