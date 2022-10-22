#ifndef MAZEPROTOTYPEFACTORY_H
#define MAZEPROTOTYPEFACTORY_H

class MazePrototypeFactory:public MazeFactory
{
	public:
		explicit MazePrototypeFactory(Maze* m,Wall* w,Room* r,Door* d):prototypeMaze(m),prototypeRoom(r),prototypeWall(w),prototypeDoor(d){}

		virtual Maze* makeMaze() const;
		virtual Maze* makeRoom(int )const;
		virtual Maze* makeWall() const {return prototypeWall->clone();}
		virtual Maze* makeDoor(Room* r1,Room* r2) const
		{
			Door* door=prototypeDoor->clone();
			door->initialize(r1,r2);
			return door;
		}
	private:
		Maze* prototypeMaze;
		Room* prototypeRoom;
		Wall* prototypeWall;
		Door* prototypeDoor;
};

#endif
