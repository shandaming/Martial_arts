#ifndef MAZEGAME_H
#define MAZEGAME_H

class MazeGame
{
	public:
		Maze* createMaze(MazeFactory& factory);
		Maze* createMaze(MazeBuilder& builder);
		Maze* createComplexMaze(MazeBuilder& builder);
};

#endif
