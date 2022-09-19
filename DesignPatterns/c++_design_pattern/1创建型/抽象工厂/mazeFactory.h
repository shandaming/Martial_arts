#ifndef MAZEFACTORY_H
#define MAZEFACTORY_H

class MazeFactory
{
	public:
		explicit MazeFactory();

		virtual Maze* makeMaze() const {return new Maze();}
		virtual Wall* makeWall() const {return new Wall();}
		virtual Room* makeRoom(int n) const {return new Room(n);}
		virtual Door* makeDoor(Room* r1,Room* r2) const {return new Door(r1,r2);}
};

#endif
