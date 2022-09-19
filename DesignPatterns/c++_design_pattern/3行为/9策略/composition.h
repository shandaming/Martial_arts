#ifndef COMPOSITION_H
#define COMPOSITION_H

class Composition
{
	public:
		explicit Composition(Compositor*);
		void repair()
		{
			coord* natural,*stretchability,shrinkability;
			int componentCount;
			int* breaks;

			int breakCount;
			breakCount=compositor->compose(natural,stretchability,shrinkability,componentCount,lineWidth,breaks);
		}
	private:
		Compositor* compositor;
		Component* components;
		int componentCount;
		int lineWidth;
		int* lineBreake;

		int lineCount;
};

#endif
