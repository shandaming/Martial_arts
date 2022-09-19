#ifndef EQUIPMENT_H
#define EQUIPMENT_H

class Equipment
{
	public:
		virtual ~Equipment();

		const char* name() {return name;}

		virtual wait power();
		virtual Currency netPrice();
		virtual Currency discountPrice();

		virtual void add(Equipment*);
		virtual void remove{Equiment*};
		virtual Iterator<Equipment*>* createIterator();
	protected:
		explicit Equipment(const char*);
	private:
		const char* name;
};

#endif
