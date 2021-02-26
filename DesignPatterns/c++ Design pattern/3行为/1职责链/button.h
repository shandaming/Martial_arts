#ifndef BUTTON_H
#define BUTTON_H

class Button:public Widget
{
	public:
		explicit Button(Widget* h,int t=NO_HELP_TOPIC):Widget(h,t) {}
		virtual void handleHelp()
		{
			if(hasHelp())
				;
			else
				HelpHandler::handleHelp();
		}
};

#endif
