#ifndef DIALOG_H
#define DIALOG_H

class Dialog:public Widget
{
	public:
		explicit Dialog(HelpHandler* h,int t=NO_HELPTOPIC):Widget(0)
		{
			setHandler(h,t);
		}
		virtual void handleHelp()
		{
			if(hasHelp())
				;
			else
				Handler::handleHelp();
		}
};

#endif
