#ifndef TEXTVIEW_H
#define TEXTVIEW_H

class TextView
{
	public:
		TextView();
		void getOrigin(Coord& x,Coord& y) const;
		void getExtent(Coord& width,Coord& height) const;
		virtual bool isEmpty() const;
};

#endif
