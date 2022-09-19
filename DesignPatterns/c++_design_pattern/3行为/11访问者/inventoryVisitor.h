#ifndef INVENTORYVISITOR_H
#define INVENTORYVISITOR_H

class InventoryVisitor:public EquipmentVisitor
{
	public:
		InventoryVisitor();

		Inventory& getInventory();

		virtual void visitFloppyDisk(FloppyDisk* e) {inventory.accumulate(e);}
		virtual void visitCard(Card*);
		virtual void visitChassis(Chassis *e) {inventory.accumulate(e);}
		virtual void visitBus(Bus*);
	private:
		Inventory inventory;
};

#endif
