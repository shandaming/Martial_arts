#ifndef PROGRAMNODEBUILDER_H
#define PROGRAMNODEBUILDER_H

class ProgramNodeBuilder
{
	public:
		ProgramNodeBuilder();

		virtual ProgramNode* newVariable(const char* variableName) const;
		virtual ProgramNode* newAssignment(ProgramNode* variable,ProgramNode* expression) const;
		virtual ProgramNode* newRetrunStatement(ProgramNode* value) const;
		virtual ProgramNode* newCondition(ProgramNode* condition,ProgramNode* truPart,ProgramNode* falsePart) const;
		ProgramNode* getRootNode();
	private:
		ProgramNode* node;
};

#endif
