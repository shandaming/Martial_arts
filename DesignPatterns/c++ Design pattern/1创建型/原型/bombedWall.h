#ifndef BOMBEDWALL_H
#define BOMBEDWALL_H

class BombedWall:public Wall
{
	public:
		BombedWall();
		explicit BombedWall(const BombedWall& other):Wall(other)
		{
			bomb=other.bomb;
		}

		virtual Wall* clone() const {return new BombedWall(*this);}
		bool hasBomb();
	private:
		bool bomb;
};

#endif
