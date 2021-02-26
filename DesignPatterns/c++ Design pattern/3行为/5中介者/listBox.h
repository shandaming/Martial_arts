#ifndef LISTBOX_H
#define LISTBOX_H

class ListBox:public Widget
{
	public:
		explicit ListBox(DialogDirect*);

		virtual const char* getSelection();
		virtual void setList(list<char*>* listItems);
		virtual void handleMouse(MouseEvent& event);
};

class EntryField:public Widget
{
	public:
		EntryField(DialogDirector*);

		virtual void setText(const char* text);
		virtual const char* getText();
		virtual void handleMouse(MouseEvent& event);
};

class Button:public Widget
{
	public:
		Button(DialogDirector*);

		virtual void setText(const char* text);
		virtual void handleMouse(MouseEvent& event)
		{
			changed();
		}
};

#endif
