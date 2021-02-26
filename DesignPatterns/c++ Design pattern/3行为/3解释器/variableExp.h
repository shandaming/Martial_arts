#ifndef VARIABLEEXP_H
#define VARIABLEEXP_H

class VariableExp:public BooleanExp
{
	public:
		explicit VariableExp(const char* n) {name=strdup(n);}
		virtual ~VariableExp();

		virtual bool evaluate(Context& c) {return c.lookup(name);}
		virtual BooleanExp* replace(const char* n,BooleanExp& exp)
		{
			if(strcmp(n,name)==0)
				return exp.copy();
			else
				return new VariableExp(name);
		}
		virtual BooleanExp* cpy() const {return new VariableExp(name);}
	private:
		char* name;
};

#endif
