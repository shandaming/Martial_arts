#include "mazeGame.h"

Maze* MazeGame::createMaze()
{
	Maze* maze=makeMaze();
	Room* r1=makeRoom(1);
	Room* r2=makeRoom(2);
	Door* door=makeDoor(r1,r2);

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
