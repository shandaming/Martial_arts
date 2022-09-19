#ifndef WIHDOW_H
#define WINDOW_H

class Window
{
	public:
		explicit Window(View* contents);

		virtual void drawContents();

		virtual void open();
		virtual void close();
		virtual void iconify();
		virtual void deiconify();

		virtual void setOrigin(const Point& at);
		virtual void setExtent(const Point& extent);
		virtual void raise();
		virtual void lower();

		virtual void drawLine(const Point&,const Point&);
		virtual void drawRect(const Point& p1,const Point& p2)
		{
			WindowImp* imp=getWindowImp();
			imp->deviceRect(p1.x(),p1.y(),p2.x(),p2.y());
		}
		virtual void drawPolygon(const Point[],int n);
		virtual void drawText(const char*,const Point&);
	protected:
		WindowImp* getWindowImp()
		{
			if(imp==0)
				imp=WindowSystemFactory::Instance()->makeWindowImp();
			return imp;
		}
		View* getView();
	private:
		WindowImp* imp;
		View* contents;
};

#endif
