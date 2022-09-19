#ifndef DIALOGDIRECTOR_H
#define DIALOGDIRECTOR_H

class DialogDirector
{
	public:
		virtual ~DialogDirector();

		virtual void showDialog();
		virtual void widgetChanged(Widget*)=0;
	protected:
		DialogDirector();
		virtual void createWidgets()=0;
};

#endif
