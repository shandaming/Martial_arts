#ifndef PROGAMNODE_H
#define PROGAMNODE_H

class ProgamNode
{
	public:
		virtual void getSourcePosition(int& line,int& index);

		virtual void add(ProgamNode*);
		virtual void remove(ProgramNode*);
	
		virtual void traverse(codeGenerator&);
	protected:
		ProgramNode();
};

#endif
