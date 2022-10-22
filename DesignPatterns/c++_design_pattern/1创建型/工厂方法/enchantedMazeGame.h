#ifndef ENCHANTEDMAZEGAME_H
#define ENCHANTEDMAZEGAME_H

class EnchantedMazeGame:public MazeGame
{
	public:
		explicit EnchantedMazeGame();

		virtual Room* makeRoom(int n) const {return new EnchantedRoom(n,castSpell());}
		virtual Door* makeDoor(Room* r1,Room* r2) const {return new DoorNeedingSpell(r1,r2);}
	protected:
		Spell* castSpell() const;
};

#endif
