#ifndef IMAGEPROXY_H
#define IMAGEPROXY_H

class ImageProxy:public Graphic
{
	public:
		explicit ImageProxy(const char* imageFile)
		{
			filename=strdup(imageFile);
			extent=Point::zero;
			image=0;
		}
		virtual ~ImageProxy();

		virtual void draw(const Point& at)
		{
			getImage()->draw();
		}
		virtual void handleMouse(Event& event)
		{
			getImage()->handleMouse(event);
		}

		virtual const Point& getExtent()
		{
			if(extent==Point::zero)
				extent=getImage()->getExtent();
			return extent;
		}

		virtual void load(istream& from)
		{
			from>>extent>>filename;
		}
		virtual void save(ostream& to)
		{
			to<<extent<<filename;
		}
	protected:
		Image* getImage()
		{
			if(image==0)
				image=new image(filename);
			return image;
		}
	private:
		Image* image;
		Point extent;
		char* filename;
};

#endif
