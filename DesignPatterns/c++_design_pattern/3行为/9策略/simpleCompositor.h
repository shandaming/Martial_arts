#ifndef SIMPLECOMPOSITOR_H
#define SIMPLECOMPOSITOR_H

class SimpleCompositor:public Compositor
{
	public:
		SimpleCompositor();

		virtual int compose(coord natural[],coord stretch[],coord shrink[],int componentCount,int lineWidth,int breaks[]);
};

#endif
