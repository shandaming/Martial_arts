#ifndef SUBJECT_H
#define SUBJECT_H

class Subject
{
	public:
		virtual ~Subject();

		virtual void attach(Observer* o)
		{
			observers.append(o);
		}
		virtual void detach(Observer* o)
		{
			observers.remove(o);
		}
		virtual void notify()
		{
			ListIterator<Observer*>i(observers);
			for(i.first();!i.isDone();i.next())
				i.currentItem()->update(this);
		}
	protected:
		Subject();
	private:
		List<Observer*>* observers;
};

#endif
