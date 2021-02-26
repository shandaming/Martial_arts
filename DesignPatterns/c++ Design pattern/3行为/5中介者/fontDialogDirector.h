#ifndef FONTDIALOGDIRECTOR_H
#define FONTDIALOGDIRECTOR_H

class FontDialogDirector:public DialogDirector
{
	public:
		FontDialogDirector();

		virtual ~FontDialogDirector();
		virtual void widgetChanged(Widget* theChangedWidget)
		{
			if(theChangedWidget==fontlist)
				fontname->setText(fontlist->getSelection());
			else if(theChangedWidget==ok)
			{}
			else if(theChangedWidget==cancel)
			{}
		}
	protected:
		virtual void createWidget()
		{
			ok=new Buttono(this);
			cancel=new Button(this);
			fontList=new ListBox(this);
			fontName=new EntryField(this);
		}
	private:
		Button* ok,*cancel;
		ListBox* fontList;
		EntryField* fontName;
};

#endif
