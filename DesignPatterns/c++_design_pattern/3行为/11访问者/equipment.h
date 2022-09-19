#ifndef EQUIPMENT_H
#define EQUIPMENT_H

class Equipment
{
	public:
		virtual ~Equipment();

		const char* name() {return name;}

		virtual Watt power();
		virtual current netPrice();
		virtual current discountPrice();

		virtual void accept(EquipmentVisitor&);
	protected:
		Equipment(const char*);
	private:
		const char* name;
};

#endif
