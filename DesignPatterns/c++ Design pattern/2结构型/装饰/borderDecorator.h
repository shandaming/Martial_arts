#ifndef BORDERDECORATOR_H
#define BORDEROECORATOR_H

class BorderDecorator:public Decorator
{
	public:
		explicit BorderDecorator(VisualComponent*,int borderWidth);

		virtual void draw() {Decorator::draw();drawBorder(width);}
	private:
		void drawBorder(int);

		int width;
};

#endif
