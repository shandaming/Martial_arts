#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

class CodeGenerator
{
	public:
		virtual void visit(statementNode*);
		virtual void visit(expressionNode*);
	protected:
		CodeGenerator(bytecodeStream&);
		bytecodeStream& output;
};

#endif
