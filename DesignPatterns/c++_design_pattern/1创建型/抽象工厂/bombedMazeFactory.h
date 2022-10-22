#ifndef BOMBEDMAZEFACTORY_H
#define BOMBEDMAZEFACTORY_H

class BombedMazeFactory:public MazeFactory
{
	public:
		Wall* makeWall() const {return new BombedWall;}
		Room* makeRoom(int n) const {return new RoomWithABomb(n);}
};

#endif
