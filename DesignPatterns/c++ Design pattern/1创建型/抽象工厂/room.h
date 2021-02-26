#ifndef ROOM_H
#define ROOM_H

class Room:public MapSite
{
	public:
		explicit Room(int roomNo);

		MapSite* getSide(direction d) const;
		void setSide(Direction d,MapSite* m);

		virtual void enter();
	private:
		MapSite* sides[4];
		int roomNumber;
};

#endif
