#ifndef XWINDOW_H
#define XWINDOW_H

class XWindow:public WindowImp
{
	public:
		XWindow();

		virtual void deviceRect(Cord x0,Coord y0,Coord x1,Coord y1)
		{
			int x=round(min(x0,x1));
			int y=round(min(y0,y1));
			int w=round(abs(x0-x1));
			int h=round(abs(y0-y1));
			XDrawRectangle(dyp,winid,gc,x,y,w,h);
		}
	private:
		Display* dpy;
		Drawable winid;
		Gc gc;
};

#endif
