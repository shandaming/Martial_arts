#ifndef TEXTSHAPE_H
#define TEXTSHAPE_H

//类适配器
class TextShape:public Shape,private TextView
{
	public:
		TextShape();

		virtual void boundingBox(Point& bottomLeft,Point& topRight) const;
		virtual bool isEmpty() const;
		virtual manipulator* createManipulator() const;
};

//对象适配器
class TextShape:public Shape
{
	public:
		TextShape(TextView*);

		virtual void boundingBox(Point& bottomLeft,Point& topRight) const;
		virtual bool isEmpty() const;
		virtual Manipulator× createManipulator() const;
	private:
		TextView* text;
};

#endif
