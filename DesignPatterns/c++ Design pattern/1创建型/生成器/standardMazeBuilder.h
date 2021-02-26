#ifndef STANDARDMAZEBUILDER_H
#define STANDARDMAZEBUILDER_H

class StandardMazeBuilder:public MazeBuilder
{
	public:
		explicit StandardMazeBuilder():currentMaze(0){}

		virtual void buildMaze() {return new Maze();}
		virtual void BuildRoom(int r);
		virtual void buildDoor(int r1,int r2);

		virtual Maze* getMaze() {return currentMaze;}
	private:
		Direction commonWall(Room* r1,Room* r2);
		Maze* currentMaze;
};

#endif
