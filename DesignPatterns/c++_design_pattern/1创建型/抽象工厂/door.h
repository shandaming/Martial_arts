#ifndef DOOR_H
#define DOOR_H

class Door:public MapSite
{
	public:
		explicit Door(Room* r1=0,Room* r2=0);

		virtual void enter();
		Room* otherSideFrom(Room* r);
	private:
		Room* room1;
		Room* room2;
		bool open;
};

#endif
