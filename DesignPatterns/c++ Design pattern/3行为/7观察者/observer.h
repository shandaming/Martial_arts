#ifndef OBSERVER_H
#define OBSERVER_H

class Observer
{
	public:
		virtual ~Observer();
		virtual void update(Subject* theChangedSubject)=0;
	protected:
		Observer();
};

#endif
