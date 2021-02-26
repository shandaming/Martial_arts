#include "standardMazeBuilder.h"

void StandardMazeBuilder::buildRoom(int n)
{
	if(!currentMaze->roomNo(n))
	{
		Room* room=new Room(n);
		currentMaze->addRoom(room);

		room->setSide(north,new Wall());
		room->setSide(south,new Wall());
		room->setSide(east,new Wall());
		room->setSide(west,new Wall());
	}
}
void StanderdMazeBuilder::buildDoor(int n1,int n2)
{
	Room* r1=currentMaze->roomNo(n1);
	Room* r2=currentMaze->roomNo(n2);
	Door* d=new Door(r1,r2);

	r1->setSide(commonWall(r1,r2),d);
	r2->setSide(commonWall(r2,r1,d));
}
