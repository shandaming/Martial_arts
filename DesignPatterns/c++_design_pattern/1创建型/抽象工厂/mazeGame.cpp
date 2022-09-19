#include "mazeGame.h"

Maze* MazeGame::createMaze(MazeFactory& factory)
{
	Maze* maze=factory.makeMaze();
	Room* r1=factory.makeRoom(1);
	Room* r2=factory.makeRoom(2);
	Door* door=factory.makeDoor(r1,r2);

	maze.addRoom(r1);
	maze.addRoom(r2);

	r1->setSide(north,factory.makeWall());
	r1->setSide(east,door);
	r1->setSide(south,factory.makeWall());
	r1->setSide(west,factory.makeWall());

	r2->setSide(north,factory.makeWall());
	r2->setSide(east,factory.makeWall());
	r2->setSide(south,factory.makeWall());
	r2->setSide(west,door);

	return maze;
}
Maze* MazeGame::createMaze(MazeBuilder& builder)
{
	builder.buildMaze();

	builder.buildRoom(1);
	builder.buildRoom(2);
	builder.buildDoor(1,2);

	return builder.getMaze();
}
Maze* MazeGame::createComplexMaze(MazeBuilder& builder)
{
	builder.buildRoom(1);
	//...
	builder.buildRoom(101);

	return builder.getMaze();
}
