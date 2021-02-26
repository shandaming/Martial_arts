#ifndef CLOCKTIMER_H
#define CLOCKTIMER_H

class ClockTimer:public Subject
{
	public:
		ClockTimer();

		virtual int getHour();
		virtual int getMinute();
		virtual int getSecond();

		void tick() {notify();}
};

#endif
