#ifndef GRAPHIC_H
#define GRAPHIC_H

class Graphic
{
	public:
		virtual ~Graphic();

		virtual void draw(const Point& at)=0;
		virtual void handleMouse(Event& event)=0;

		virtual const Point& getExtent()=0;

		virtual void load(istream& from)=0;
		virtual void save(ostream& to)=0;
	protected:
		Graphic();
};

#endif
