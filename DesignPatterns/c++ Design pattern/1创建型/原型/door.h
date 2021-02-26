#ifndef DOOR_H
#define DOOR_H

class Door:public MapSite
{
	public:
		explicit Door();
		explicit Door(const Door& other)
		{
			room1=other.room1;
			room2=other.room2;
		}

		virtual void initialize(Room*,Room*) {room1=r1;room2=r2;}
		virtual Door* clone() const {return new Door(*this);}
		virtual void enter();
		Room* otherSideFrom(Room*);
	private:
		Room* room1,*room2;
};

#endif`
