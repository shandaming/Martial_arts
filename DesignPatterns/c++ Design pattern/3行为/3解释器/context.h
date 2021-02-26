#ifndef CONTEXT_H
#define CONTEXT_H

class Context
{
	bool lookup(const char*) const;
	void assign(VarlabelExp*,bool);
};

#endif
