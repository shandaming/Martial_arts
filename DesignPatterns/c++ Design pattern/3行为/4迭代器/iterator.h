#ifndef ITERATOR_H
#define ITERATOR_H

template <typename Item>
class Iterator
{
	public:
		virtual void first()=0;
		virtual void next()=0;
		virtual void isDone() const=0;
		virtual Item currentItem() const=0;
	protected:
		Iterator();
};

#endif
