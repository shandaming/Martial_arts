#ifndef SHAPE_H
#define SHAPE_H

class Shape
{
	public:
		Shape();
		virtual void boundingBox(Point& bottomLeft,Point& topRight) const;
		virtual manipulator* createManipulator() const;
};

#endif
