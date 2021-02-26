#ifndef LIST_H
#define LIST_H

template <typename Item>
class List
{
	public:
		explicit List(long size=DEFAULT_LIST_CAPACITY);

		long count() const;
		Item& get(long index) const;
};

#endif
