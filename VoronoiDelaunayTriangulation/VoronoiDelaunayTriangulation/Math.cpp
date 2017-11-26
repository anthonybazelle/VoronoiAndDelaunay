#include "Math.h"

bool Math::isPointVisible(maths::Point polPoint, maths::Point winPoint, maths::Point n)
{
	return ((n.x*(polPoint.x - winPoint.x) + n.y*(polPoint.y - winPoint.y))>=-0.001);
}



maths::Point Math::getIntersection(maths::Point p1, maths::Point p2, maths::Point p3, maths::Point p4)
{
	maths::Point p;

	float x = p2.x - p1.x;
	float x2 = p2.y - p1.y;
	float y = p3.x - p4.x;
	float y2 = p3.y - p4.y;
	float z = p3.x - p1.x;
	float z2 = p3.y - p1.y;
	int s = (z2*x - z*x2) / (y2*x - y*x2);
	int t = (z - s*y) / x;


	std::cout << "s=" << s << "		t=" << t << std::endl;
	if (s < 0 || s>1 || t < 0 || t>1)
	{
		p.x = -1;
		p.y = -1;
		return p;
	}

	p.x = p1.x + x*t;
	p.y = p1.y + x2*t;
	return p;
}

maths::Point Math::getIntersection2(const maths::Point p1, const maths::Point p2, const maths::Point p3, const maths::Point p4)
{
	maths::Point result;
	maths::Point v1;
	v1.x = p2.x - p1.x;
	v1.y = p2.y - p1.y;
	maths::Point v2;
	v2.x = p4.x - p3.x;
	v2.y = p4.y - p3.y;
	float m(0), k(0), diviseur(v1.x * v2.y - v1.y * v2.x);

	if (diviseur != 0)
	{
		m = (v1.x * p1.y
			- v1.x * p3.y
			- v1.y * p1.x
			+ v1.y * p3.x
			) / diviseur;
		k = (v2.x * p1.y
			- v2.x * p3.y
			- v2.y * p1.x
			+ v2.y * p3.x
			) / diviseur;
		result.x = p3.x + m*v2.x; 
		result.x = Math::round(result.x);
		result.y = p3.y + m*v2.y;
		result.y = Math::round(result.y);
		float minV1, maxV1, minV2, maxV2;
		if (p1.x > p2.x)
		{
			minV1 = p2.x;
			maxV1 = p1.x;
		}
		else
		{
			minV1 = p1.x;
			maxV1 = p2.x;
		}
		if (p3.x > p4.x)
		{
			minV2 = p4.x;
			maxV2 = p3.x;
		}
		else
		{
			minV2 = p3.x;
			maxV2 = p4.x;
		}
		if(result.x>minV1 && result.x<maxV1 && result.x>minV2 && result.x<maxV2)
			return result;
	}
	result.x =-1;
	result.y = -1;
	return result;
}

bool Math::isPointInPol(maths::Polygon pol, maths::Point p)
{
	pol.calculateNormals();
	int nbPointWin = pol.getPoints()->size();
	//ici ton point p qui correspond à là ou t'a cliqué
	for (int j = 0; j < nbPointWin; j++)
	{
		maths::Point p1 = pol.getPoints()->at(j);

		if (!Math::isPointVisible(p, p1, pol.getNormals()->at(j)))
			return false;
	}
	return true;
}

float Math::round(float nb)
{
	nb *= 1000;
	int r = nb;
	nb = r;
	return nb / 1000;
}

