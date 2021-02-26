#ifndef APPLICATION_H
#define APPLICATION_H

class Application:public HelpHandler
{
	public:
		explicit Application(int t):HelpHandler(0,t) {}

		virtual void handleHelp() {}
};

#endif
