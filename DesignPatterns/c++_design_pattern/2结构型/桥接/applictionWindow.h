#ifndef APPLICATIONWINDOW_H
#define APPLICATIONWINDOW_H

class ApplicationWindow:public Window
{
	public:
		virtual void drawContents() {getView()->drawOn(this);}
};

#endif
