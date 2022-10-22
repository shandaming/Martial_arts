#ifndef BOOLEANEXP_H
#define BOOLEANEXP_H

class BooleanExp
{
	public:
		BooleanExp();
		virtual ~BooleanExp();

		virtual bool evaluate(Cootext&)=0;
		virtual BooleanExp* replace(const char*, BooleanExp&)=0;
		virtual BooleanExp* copy() const=0;
};

#endif
