#ifndef PASTECOMMAND_H
#define PASTECOMMAND_H

class PasteCommand:public Command
{
	public:
		explicit PasteCommand(Document* doc) {document=doc;}

		virtual void execute() {document->paste();}
	private:
		Document* document;
};

#endif
