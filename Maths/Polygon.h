#ifndef DEF_MATHS_POLYGON
#define DEF_MATHS_POLYGON

#include <vector>
#include <iostream>

namespace maths
{
	class Triangle;
	class Line;

	struct Point
	{
		float x;
		float y;
		float z;


		float Point::Distance(maths::Point p2)
		{
			return sqrt(pow(p2.x - x, 2) + pow(p2.y - y, 2));
		}

		float Point::Angle(maths::Point indice2)
		{
			return atan2(y - indice2.y, x - indice2.x);
		}

		bool equals2D(Point* p)
		{
			return (p->x == x && p->y == y);
		}
	};

	class Triangle
	{
	public:
		Line *l1, *l2, *l3;

		float sign(Point* p1, Point* p2, Point* p3);
		bool pointInTriangle(Point* pt);
	};

	struct Droite
	{
		float a, b;

		Point intersection(Droite droite)
		{
			Point p = { -2,-2,0 };
			if (a == droite.a)
				return p;
			float x = (droite.b - b) / (a - droite.a);
			p.x = x;
			p.y = a*p.x + b;

			return p;
		}
	};

	class Line
	{
	public:
		Point *p1, *p2;
		Triangle *droite, *gauche;

		Point middle()
		{
			Point *first = firstPoint(), second = *secondPoint();
			return{ (first->x + second.x) / 2, (first->y + second.y) / 2 };
		}

		bool isVertical()
		{
			return p1->x == p2->x;
		}

		bool isHorizontal()
		{
			return p1->y == p2->y;
		}

		bool isCol(Point* p)
		{
			if (isVertical())
				return p->x == p1->x;
			return p->y == coefDirecteur()*p->x + num();
		}

		//attention ne marche par avec les droite horozontales
		bool equals(Line* line)
		{
			return ((p1->equals2D(line->p1) && p2->equals2D(line->p2)) || (p2->equals2D(line->p1) && p1->equals2D(line->p2)));
		}

		float coefDirecteur()
		{
			if (p1->y==p2->y)
			{
				return 0;
			}
			if (p1->x < p2->x)
			{
				return (p2->y - p1->y) / (p2->x - p1->x);
			}
			else if (p1->x > p2->x)
			{
				return (p1->y - p2->y) / (p1->x - p2->x);
			}
			return -1;
		}

		float num(float coef)
		{
			if (coef = -1)
				return -1;
			return p1->y - coef*p1->x;
		}

		float num()
		{
			return num(coefDirecteur());
		}

		Point* firstPoint()
		{
			if (p1->x < p2->x)
				return p1;
			else return p2;
		}

		Point* secondPoint()
		{
			if (p1->x < p2->x)
				return p2;
			else return p1;
		}

		Point* firstPointY()
		{
			if (p1->y < p2->y)
				return p1;
			else return p2;
		}

		Point* secondPointY()
		{
			if (p1->y < p2->y)
				return p2;
			else return p1;
		}
	};

	struct Color
	{
		float r;
		float g;
		float b;
	};

	class Polygon
	{
		std::vector<maths::Point*> *points;
		std::vector<maths::Point> *normals;
		std::vector<maths::Point> *vectors;
		std::vector<bool> *visibility;
		bool sensTrigo;
		void recursiveRecalculateBezierPoints();
		void recursiveRecalculateBezierPointsCoxDeBoor(int i, std::vector<maths::Point> *tmp);
		float coxDeBoor(float u, int i, int k, const float* Knots);
		std::vector<maths::Point> *bezierPoints;
		maths::Polygon *outPolygon;
		maths::Polygon *inPolygon;
		int bezierRecursion;

		unsigned int g_num_cvs;
		unsigned int g_degree;
		unsigned int g_order;
		unsigned int g_num_knots;

	public:
		void setOutPolygon(maths::Polygon *pol);
		void setInPolygon(maths::Polygon *pol);
		maths::Polygon* getOutPolygon();
		maths::Polygon* getInPolygon();

		void addPoint(maths::Point* p, int index);
		void addPoint(maths::Point* p);
		maths::Point* getPoint(int i);
		void removePoint();
		void removePoint(int index);
		void setVisibility(int index, bool visible);
		bool isPointVisible(int index);
		void setPoint(maths::Point* p, int indice);
		const std::vector<maths::Point>* getNormals();
		Polygon();
		~Polygon();
		std::vector<maths::Point*>* getPoints();
		std::vector<maths::Point>* getBezierPoints();
		void changeBezierRecursion(int nb);
		int factorial(int n);
		float binomial_coff(float n, float k);
	};
}


#endif // !MATHS_POLYGON