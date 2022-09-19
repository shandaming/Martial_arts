#ifndef HELPHANDLER_H
#define HELPHANDLER_H

const int NO_HELP_TOPIC=-1;

class HelpHandler
{
	public:
		explicit HelpHandler(HelpHandler* h=0,int t=NO_HELP_TOPIC):successor(j),topic(t) {}
		virtual bool hasHelp() {return topic!=NO_HELP_TOPIC;}
		virtual void setHandler(HelpHandler* topic);
		virtual void handlerHelp()
		{
			if(successor!=0)
				successor->handleHelp();
		}
	private:
		HelpHandler* successor;
		int topic;
};

#endif
