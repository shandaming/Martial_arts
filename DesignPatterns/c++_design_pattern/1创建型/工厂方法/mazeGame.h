#ifndef MAZEGAME_H
#define MAZEGAME_H

class MazeGame
{
	public:
		Maze* createMaze();

		virtual Maze* makeMaze() const {return new Maze();}
		virtual Room* makeRoom(int n) const {return new Room(n);}
		virtual Wall* makeWall() const {return new Wall();}
		virtual Door* makeDoor(Room* r1,Room* r2) const {return new Door(r1,r2);}
};

#endif
