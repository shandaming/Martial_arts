#ifndef MACROCOMMAND_H
#define MACROCOMMAND_H

class MacroCommand:public Command
{
	public:
		MacroCommand();
		virtual ~MacroCommand();

		virtual void add(Command* c)
		{
			cmds->append(c);
		}
		virtual void remove(Command*)
		{
			cmds->remove(c);
		}

		virtual void execute()
		{
			for(i.fist();!i.isDone();i.next())
			{
				Command* c=i.currentItem();
				c->execute();
			}
		}
	private:
		std::list<Command*> cmds;
};

#endif
