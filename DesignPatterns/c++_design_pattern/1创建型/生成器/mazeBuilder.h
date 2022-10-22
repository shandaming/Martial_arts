#ifndef MAZEBUILDER_H
#define MAZEBUILDER_H

class MazeBuilder
{
	public:
		virtual void buildMaze() {}
		virtual void buildRoom(int room) {}
		virtual void buildDoor(int roomFrom,int roomTo) {}

		virtual Maze* getMaze() {return 0;}
	protected:
		explicit MazeBuilder();
};

#endif
