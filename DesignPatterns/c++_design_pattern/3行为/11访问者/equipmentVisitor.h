#ifndef EQUIPMENTVISITOR_H
#define EQUIPMENTVISITOR_H

class EquipmentVisitor
{
	public:
		virtual ~EquipmentVisitor();

		virtual void visitFloppyDisk(FioppyDisk*);
		virtual void visitCard(Card*);
		virtual void visitChassis(chassis*);
		virtual void visitBus(Bus*);
	protected:
		EquipmentVisitor();
};

#endif
