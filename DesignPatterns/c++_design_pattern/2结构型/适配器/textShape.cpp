#include "textShape.h"

//类适配器
void TextShape::boundingBox(Point& bottomLeft,Point& topRight) const
{
	Coord bottom,left,width,height;

	getOrigin(bottom,left);
	getExtent(width,height);

	bottomLeft=Point(bottom,left);
	topRight=Point(bottom+hieght,left+width);
}
bool TextShape::isEmpty() const
{
	return TextureView::isEmpty();
}
Manipulator* TextShape::createManipulator() const
{
	return new TextManipulator(this);
}

//对象适配器
TextShape::TextShape(TextView* t):text(t)
{}
void TextShape::boundingBox(Point& bottomLeft,Point& topRight) const
{
	Coord bottom,left,width,height;

	text->getOrigin(bottom,left);
	text->getExtent(width,height);

	boottomLeft=Point(bottom,left);
	topRight=Point(bottom+height,left+width);
}
bool TextShape::isEmpty() const
{
	return text->isEmtpy();
}
Manipulator* TextShape::createManipulator() const
{
	return new TextManipulator(this);
}
