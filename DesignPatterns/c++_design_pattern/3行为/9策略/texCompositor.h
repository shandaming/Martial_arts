#ifndef TEXCOMPOSITOR_H
#define TEXCOMPOSITOR_H

class TexCompositor:public Compositor
{
	public:
		TexCompositor();

		virtual int compose(coord natural[],coord stretch[],coord shrink[],int componentCount,int lineWidth,int breaks[]);
};

#endif
