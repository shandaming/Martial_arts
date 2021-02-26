#ifndef WIDGET_H
#define WIDGET_H

class Widget
{
	public:
		Widget(DialogDirector*);
		virtual void changed() {director->widgetChanged(this);}

		virtual void handleMouse(MouseEvent& event);
	private:
		DialogDirector* director;
};

#endif
