#ifndef CONSTRAINTSOLVER_H
#define CONSTRATINTSOLVER_H

class ConstraintSolver
{
	public:
		static ConstraintSolver* instance();

		void solve();
		void addConstraint(Graphic* startConnection,Graphic* endConnection);
		void removeConstaraint(Graphic* startConnection,Graphic* endConnection);
	private:
};

#endif
