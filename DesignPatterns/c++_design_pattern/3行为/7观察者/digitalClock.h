#ifndef diGITALCLOCK_H
#define DIGITALCLOCK_H

class DigitalClock:public Widget,public Observer
{
	public:
		DigitalClock(ClockTimer* s)
		{
			subject=s;
			subject->attach(this);
		}
		virtual ~DigitalClock() {subject->detach(this);}

		virtual void update(Subject*)
		{
			if(theChangedSubject==subject)
				draw();
		}
		virtual void draw()
		{
			int hour=subject->getHour();
			int minute=subject->getMinute();
		}
	private:
		ClockTimer* subject;
};

#endif
