#ifndef LISTITERATOR_H
#define LISTITERATOR_H

template <typename Item>
class ListIterator:public Iterator<Item>
{
	public:
		explicit ListIterator(const List<Item*> alist):list(alist),current(0) {}
		virtual void first() {current=0;}
		virtual void next() {++current;}
		virtual void isDone() const {return current>=list->count();}
		virtual void currentItem() const
		{
			if(isDone())
				throw iteratorOutOfBounds;
			return list->get(current);
		}
	private:
		const List<Item*> list;
		long current;
};

#endif
