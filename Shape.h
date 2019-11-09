#pragma once
#include "Struct.h"
class Shape
{
public:
	virtual bool collision_detection(Dot p0) = 0;
};
class Line :public Shape
{
public:
	Dot p1, p2;
	Line(Dot pp1, Dot pp2);
	bool collision_detection(Dot p0);
};
class Circle :public Shape
{
public:
	Dot p1;
	double r;
	Circle(Dot pp1, double rr);
	bool collision_detection(Dot p0);
};
