#ifndef COMPOSITOR_H
#define COMPOSITOR_H

class Compositor
{
	public:
		virtual int compose(coord natural[],coord stretch[],coord shrink[],int componentCount,int lineWidth,int breaks[])=0;
	protected:
		Compositor();
};

#endif
