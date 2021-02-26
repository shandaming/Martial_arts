#ifndef IMAGE_H
#define IMAGE_H

class Image:public Graphic
{
	public:
		explicit Image(const char* file);
		virtual ~Image();

		virtual void draw(const Point& at);
		virtual void handleMouse(Event& evnet);

		virtual const Point& getExtent();

		virtual void load(istream& from);
		virtual void save(ostream& to);
	private:
};

#endif
