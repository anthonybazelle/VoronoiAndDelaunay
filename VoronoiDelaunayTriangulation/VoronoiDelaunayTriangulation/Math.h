#ifndef DEF_MATHS
#define DEF_MATHS

#include "Polygon.h"

#include <iostream>

class Math
{
	


public:
	static bool isPointVisible(maths::Point polPoint, maths::Point winPoint, maths::Point n);
	static maths::Point getIntersection(maths::Point p1, maths::Point p2, maths::Point p3, maths::Point p4);
	static maths::Point getIntersection2(const maths::Point _A, const maths::Point _B, const maths::Point _C, const maths::Point _D);
	static bool isPointInPol(maths::Polygon pol, maths::Point p);
	static float round(float nb);
};

#endif

