#ifndef PRICINGVISITOR_H
#define PRICINGVISITOR_H

class PricingVisitor:public: EquipmentVisitor
{
	public:
		PricingVisitor();

		currency& getTotalPrice();

		virtual void visitFloppyDisk(FloppyDisk* e) {totl+=e->netPrice();}
		virtual void visitCard(card*);
		virtual void visitChassis(chassis* e) {total+=e->discountPrice();}
		virtual void visitBus(Bus*);
	private:
		Currency total;
};

#endif
