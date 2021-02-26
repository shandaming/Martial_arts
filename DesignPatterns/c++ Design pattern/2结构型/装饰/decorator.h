#ifndef DECORATOR_H
#define DECORATOR_H

class Decorator:public VisualComponent
{
	public:
		explicit Decorator(VisualComponent*);

		virtual void draw() {component->draw();}
		virtual void resize() {component->resize();}
	private:
		VisualComponent* component;
};

#endif
