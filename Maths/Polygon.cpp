#include "Polygon.h"

float g_Knots[] = { 0.0f,0.0f,0.0f,0.0f,0.25f,0.5f,0.75f,1.0f,1.0f,1.0f,1.0f };

void maths::Polygon::calculateNormals()
{
	if (normals->empty() && points->size() > 1)
	{
		int size = points->size();
		for (int i = 0; i < size ; i++)
		{
			maths::Point n;
			maths::Point p1= points->at(i);
			maths::Point p2;
			if (i == size - 1)
				p2 = points->at(0);
			else 
				p2 = points->at(i+1);
			if (sensTrigo)
			{
				n.x = (p1.y - p2.y);
				n.y = -(p1.x - p2.x);
			}	
			else
			{
				n.x = -(p1.y - p2.y);
				n.y = (p1.x - p2.x);
			}
			normals->push_back(n);
		}
	}
}

void maths::Polygon::calculateVectors()
{
	if (vectors->empty() && points->size() > 1)
	{
		for (int i = 1; i < points->size(); i++)
		{
			maths::Point v;
			v.x = (points->at(i).x - points->at(i-1).x);
			v.y = (points->at(i).y - points->at(i-1).y);
			vectors->push_back(v);
		}
	}
}

void maths::Polygon::addPoint(maths::Point p)
{
	if(points->size()==1)
	{
		sensTrigo = p.x < points->at(0).x;
	}
	points->push_back(p);
	visibility->push_back(true);
}

void maths::Polygon::addPoint(maths::Point p, int index)
{
	points->insert(points->begin()+index, p);
	visibility->insert(visibility->begin() + index, true);
}



int maths::Polygon::factorial(int n)
{
	if (n <= 1)
		return(1);
	else
		n = n*factorial(n - 1);
	return n;
}

float maths::Polygon::binomial_coff(float n, float k)
{
	float ans;
	ans = factorial(n) / (factorial(k)*factorial(n - k));
	return ans;
}

void maths::Polygon::removePoint()
{
	if (!points->empty())
	{
		points->pop_back();
		visibility->pop_back();
	}
		
}

void maths::Polygon::removePoint(int index)
{
	points->erase(points->begin() + index);
	visibility->erase(visibility->begin() + index);
}

void maths::Polygon::setVisibility(int index, bool visible)
{
	visibility->at(index) = visible;
}

bool maths::Polygon::isPointVisible(int index)
{
	return visibility->at(index);
}

std::vector<maths::Point>* maths::Polygon::getPoints()
{
	return points;
}

const std::vector<maths::Point>* maths::Polygon::getNormals()
{
	return normals;
}

void maths::Polygon::setPoint(maths::Point p, int indice)
{
	this->points->at(indice) = p;

	if (outPolygon != NULL && indice == points->size() - 1)
	{
		outPolygon->getPoints()->at(0) = p;
	}
	else if (inPolygon != NULL && indice == 0 )
	{
		inPolygon->getPoints()->at(inPolygon->getPoints()->size()-1) = p;
	}
}

maths::Polygon::Polygon()
{
	points = new std::vector<maths::Point>();
	normals = new std::vector<maths::Point>();
	vectors = new std::vector<maths::Point>();
	bezierPoints = new std::vector<maths::Point>();
	visibility = new std::vector<bool>();
	outPolygon = NULL;
	inPolygon = NULL;
	bezierRecursion = 10;

}

void maths::Polygon::setOutPolygon(maths::Polygon *pol)
{
	outPolygon = pol;
}


void maths::Polygon::setInPolygon(maths::Polygon *pol)
{
	inPolygon = pol;
}

maths::Polygon* maths::Polygon::getOutPolygon()
{
	return outPolygon;
}

maths::Polygon* maths::Polygon::getInPolygon()
{
	return inPolygon;
}

void maths::Polygon::recalculateBezierPointsCoxDeBoor()
{
	bezierPoints->clear();
	for (int i = 0; i < points->size(); i++)
	{
		bezierPoints->push_back(points->at(i));
	}

	g_num_cvs = points->size();
	g_degree = 3;
	g_order = g_degree + 1;
	g_num_knots = g_num_cvs + g_order;

	std::vector<maths::Point> *tmp = new std::vector<maths::Point>();

	for (int i = 0; i != bezierRecursion; ++i) {
		recursiveRecalculateBezierPointsCoxDeBoor(i, tmp);
	}

	bezierPoints->clear();
	for (int i = 0; i < tmp->size(); i++)
	{
		bezierPoints->push_back(tmp->at(i));
	}
	
	delete tmp;
}

maths::Point maths::Polygon::calculateBezierPoints(double t) {
	maths::Point P;
	P.x = 0; P.y = 0;
	int size = points->size();
	for (int i = 0; i<size; i++)
	{
		P.x = P.x + binomial_coff((float)(size - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (size - 1 - i)) * points->at(i).x;
		P.y = P.y + binomial_coff((float)(size - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (size - 1 - i)) * points->at(i).y;
	}
	return P;
}

void maths::Polygon::recalculateBezierPoints(int width, int height)
{
	if (points->size() <= 0)
		return;

	bezierPoints->clear();
	maths::Point p1 = points->at(0);
	bezierPoints->push_back(p1);
	double step = 0.2 / bezierRecursion;
	for (double t = 0.0; t <= 1.0; t += step)
	{
		Point p2 = calculateBezierPoints( t);

		bezierPoints->push_back(p2);
		p1 = p2;
	}
	bezierPoints->push_back(points->at(points->size()-1));
}




maths::Point maths::Polygon::getPoint(int i)
{
	return points->at(i);
}

void  maths::Polygon::changeBezierRecursion(int nb)
{
	bezierRecursion += nb;
	if (bezierRecursion > 10)
		bezierRecursion = 10;
	if (bezierRecursion < 1)
		bezierRecursion = 1;
}

float maths::Polygon::coxDeBoor(float u, int i, int k, const float* Knots)
{
	if (k == 1)
	{
		if (Knots[i] <= u && u <= Knots[i + 1]) {
			return 1.0f;
		}
		return 0.0f;
	}
	float Den1 = Knots[i + k - 1] - Knots[i];
	float Den2 = Knots[i + k] - Knots[i + 1];
	float Eq1 = 0, Eq2 = 0;
	if (Den1>0) {
		Eq1 = ((u - Knots[i]) / Den1) * coxDeBoor(u, i, k - 1, Knots);
	}
	if (Den2>0) {
		Eq2 = (Knots[i + k] - u) / Den2 * coxDeBoor(u, i + 1, k - 1, Knots);
	}
	return Eq1 + Eq2;
}

void maths::Polygon::recursiveRecalculateBezierPointsCoxDeBoor(int i, std::vector<maths::Point> *tmp)
{
	float t = g_Knots[g_num_knots - 1] * i / (float)(bezierRecursion - 1);

	if (i == bezierRecursion - 1)
		t -= 0.001f;

	maths::Point outPoint;
	outPoint.x = outPoint.y = 0;

	for (unsigned int i = 0; i != g_num_cvs; ++i) {

		// calculate the effect of this point on the curve
		float Val = coxDeBoor(t, i, g_order, g_Knots);

		if (Val>0.001f) {

			// sum effect of CV on this part of the curve
			outPoint.x += Val * bezierPoints->at(i).x;
			outPoint.y += Val * bezierPoints->at(i).y;
		}
		std::cout << "x=" << outPoint.x << "    y=" << outPoint.y << std::endl;
		tmp->push_back(outPoint);
	}
}

void maths::Polygon::recursiveRecalculateBezierPoints()
{
	std::vector<maths::Point> *tmp = new std::vector<maths::Point>();

	tmp->push_back(bezierPoints->at(0));

	for (int i = 0; i < bezierPoints->size() - 1; i++)
	{
		Point newPoint;
		Point p1 = bezierPoints->at(i);
		Point p2 = bezierPoints->at(i + 1);
		if (p1.x < p2.x)
			newPoint.x = p1.x + (p2.x - p1.x) / 2;
		else newPoint.x = p2.x + (p1.x - p2.x) / 2;

		if (p1.y < p2.y)
			newPoint.y = p1.y + (p2.y - p1.y) / 2;
		else newPoint.y = p2.y + (p1.y - p2.y) / 2;

		tmp->push_back(newPoint);
	}
	tmp->push_back(bezierPoints->at(bezierPoints->size() - 1));

	bezierPoints->clear();
	for (int i = 0; i < tmp->size(); i++)
	{
		bezierPoints->push_back(tmp->at(i));
	}

	delete tmp;
}


std::vector<maths::Point>* maths::Polygon::getBezierPoints()
{
	return bezierPoints;
}


maths::Polygon::~Polygon()
{
}
