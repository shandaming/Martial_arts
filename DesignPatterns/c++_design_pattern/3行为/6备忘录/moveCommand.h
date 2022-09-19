#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

class MoveCommand
{
	public:
		MoveCommand(Graph* targe,const Point& delta);
		void execute()
		{
			ConstraintSolver* solver=ConstraintSolver::Instance();
			state=solver->createMemento();
			target->move(delta);
			solver->solver();
		}
		void unexecute()
		{
			ConstraintSolver* solver=ConstraintSolver::Instance();
			target->move(delta);
			solver->setMemento(state);
			solver->solve();
		}
	private:
		ConstraintSolverMemento* state;
		Point delta;
		Graphic* target;
};

#endif
