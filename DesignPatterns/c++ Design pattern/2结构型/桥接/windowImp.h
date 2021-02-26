#ifndef WINDOWIMP_H
#define WINDOWIMP_H

class WindowImp
{
	public:
		virtual void impTop()=0;
		virtual void impBottom()=0;
		virtual void impSetExtent(const Point&)=0;
		virtual void impSetOrigin(const Point&)=0;

		virtual void deviceRect(Coord,Coord,Coord,Coord)=0;
		virtual void deviceText(const char*,Coord,Coord)=0;
		virtual void DeviceBitmap(const char*,Coord,Coord)=0;
	protected:
		WindowImp();
};

#endif
