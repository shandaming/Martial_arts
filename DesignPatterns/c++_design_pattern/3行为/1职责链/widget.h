#ifndef WIDGET_H
#define WIDGET_H

class Widget:public HelpHandler
{
	protected:
		explicit Widget(Widget* parent,int t=NO_HELP_TOPIC):HelpHandler(parent,t) {return parent;}
	private:
		Widget* parent;
};

#endif
