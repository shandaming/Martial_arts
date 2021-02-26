#ifndef SIMPLECOMMAND_H
#define SIMPLECOMMAND_H

template <typename T>
class SimpleCommand:public Command
{
	public:
		typedef void (T::*action)();

		explicit SimpleCommand(T* r,Action a):reveiver(r),action(a) {}

		virtual void execute();
	private:
		Action action;
		T* reveiver;
};

template <typename T>
void SimpleConnad::execute()
{
	(receiver->*action)();
}

#endif
