#include "stdafx.h"
#include "Shape.h"

Line::Line(Dot pp1, Dot pp2)
{
	p1 = pp1;
	p2 = pp2;
}

bool Line::collision_detection(Dot p0)
{
	double fenzi_1 = ((double)p1.second - (double)p2.second) * (double)p0.first - ((double)p1.first - (double)p2.first) * (double)p0.second - ((double)p1.second - (double)p2.second) * (double)p1.first + ((double)p1.first - (double)p2.first) * (double)p1.second;
	fenzi_1 = abs(fenzi_1);
	double p1_p2_x = (double)p2.first - (double)p1.first;
	double p1_p2_y = (double)p2.second - (double)p1.second;
	double fenmu_2 = p1_p2_x * p1_p2_x + p1_p2_y * p1_p2_y;
	double fenmu_1 = sqrt(fenmu_2);
	double res_1 = fenzi_1 / fenmu_1;
	double fenzi_2 = (double)p0.first * pow(((double)p1.first - (double)p2.first), 2) + (double)p1.first * pow(((double)p1.second - (double)p2.second), 2) + ((double)p0.second - (double)p1.second) * ((double)p1.first - (double)p2.first) * ((double)p1.second - (double)p2.second);
	double res_2 = fenzi_2 / fenmu_2;
	double a, b;
	if (p1.first < p2.first)
	{
		a = (double)p1.first;
		b = (double)p2.first;
	}
	else
	{
		a = (double)p2.first;
		b = (double)p1.first;
	}
	if (res_1< 5 && res_2>a - 5 && res_2 < b + 5)
		return true;
	else
		return false;
}

Circle::Circle(Dot pp1, double rr)
{
	p1 = pp1;
	r = rr;
}

bool Circle::collision_detection(Dot p0)
{
	double p0_p1_x = (double)p1.first - (double)p0.first;
	double p0_p1_y = (double)p1.second - (double)p0.second;
	double f = sqrt(p0_p1_x * p0_p1_x + p0_p1_y * p0_p1_y);
	double res = abs(f - r);
	if (res < 5)
		return true;
	else
		return false;
}
