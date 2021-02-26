#ifndef MAPSITE_H
#define MAPSITE_H

enum Direction(north,south,east,west);

class MapSite
{
	public:
		virtual void enter()=0;
};

#endif
