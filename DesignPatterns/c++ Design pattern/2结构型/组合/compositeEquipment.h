#ifndef COMPOSITEEQUIPMENT_H
#define COMPOSITEEQUIPMENT_H

class CompositeEquipment:public Equiment
{
	public:
		virtual ~CompositeEquipment();

		virtual wait power();
		virtual Currency netPrice()
		{
			Iterator<Equipment*>* i=createIterator();
			Currency total=0;

			for(i->first();!i->isDone();i->next())
				total+=i->currentItem()->netPrice();
			delete i;
			return total;
		}
		virtual Currency discountPrice();

		virtual void add(Equipment*);
		virtual void remove(Equipment*);
		virtual Iterator<Equipment*>* cureatIterator();
	protected:
		explicit CompositeEquipment(const char*);
	private:
		List<Equipment*> equipment;
};

#endif
