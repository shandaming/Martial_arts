#ifndef ANDEXP_H
#define ANDEXP_H

class AndExp:public BooleanExp
{
	public:
		explicit AndExp(BooleanExp* op1,BooleanExp* op2):operand1(op1),operand2(op2) {}
		virtual ~AndExp();

		virtual bool evaluate(Context& c) {return operand1->evaluate(c) && operand2->evaluate(c);}
		virtual BooleanExp* replace(const char* n,BooleanExp& exp) {return new AndExp(operand1->replace(n,exp),operand2->replace(n,exp));}
		virtual BooleanExp* copy() const {return new AndExp(operand1->copy(),operand2->copy());}
	private:
		BooleanExp* operand1,operand2;
};

#endif
