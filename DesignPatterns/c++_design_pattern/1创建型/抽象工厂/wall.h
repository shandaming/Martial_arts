#ifndef WALL_H
#define WALL_H

class Wall:public MapSite()
{
	public:
		explicit Wall();

		virtual void enter();
};

#endif
