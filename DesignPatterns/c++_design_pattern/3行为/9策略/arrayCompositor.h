#ifndef ARRAYCOMPOSITOR_H
#define ARRAYCOMPOSITOR_H

class ArrayCompositor:public Compositor
{
	ArrayCompositor(int interval);

	virtual int compose(coord natural[],coord stretch[],coord shrink[],int compoinetCount,int lineWidth,int breaks[]);
};

#endif
