#ifndef OPENCOMMAND_H
#define OPENCOMMAND_H

class OpenCommand:public Command
{
	public:
		explicit OpenCommand(Application* a) {application=a;}

		virtual void execute()
		{
			const char* name=askUser();
			if(name!=0)
			{
				Document* document=new Document(name);
				application->add(document);
				document->open();
			}
		}
	protected:
		virtual const char* askUser();
	private:
		Application application;
		char* responise;
};

#endif
