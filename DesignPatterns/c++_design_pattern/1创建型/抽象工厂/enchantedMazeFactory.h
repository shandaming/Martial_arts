#ifndef ENCHANTEDMAZEFACTORY_H
#define ENCHANTEDMAZEFACTORY_H

class EnchantedMazeFacotyr:public MazeFactory
{
	public:
		explicit EnchantedMazeFactory();

		virtual Room* makeRoom(int n) const {return new EnchantedRoom(n,castSpell());}
		virtual Door* makeDoor(Room* r1,Room* r2) const {return new DoorNeedingSpell(r1,r2);}
	protected:
		Spell* castSpell() const;
};

#endif
