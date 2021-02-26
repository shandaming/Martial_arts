#ifndef BOMBEDMAZEGAME_H
#define BOMBEDMAZEGAME_H

class BombedMazeGame:public MazeGame
{
	public:
		explicit BombedMazeGame();

		virtual Wall* makeWall() const {return new BombedWall();}
		virtual Room* makeRoom(int n) const {return new RoomWithABomb(n);}
};

#endif
