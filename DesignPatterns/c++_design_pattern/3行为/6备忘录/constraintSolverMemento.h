#ifndef CONSTRAINTSOLVERMEMENTO_H
#define CONSTRAINTSOLVERMEMENT_H

class ConstraintSolverMemento
{
	public:
		virtual ~ConstraintSolverMemento();
	private:
		friend class ConstraintSolver;
		ConstraintSolverMemento();
};

#endif
