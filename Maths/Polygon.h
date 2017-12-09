#ifndef DEF_MATHS_POLYGON
#define DEF_MATHS_POLYGON

#include <vector>
#include <iostream>

namespace maths
{
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
		bool equals2D(Point p)
		{
			return (p.x == x && p.y == y);
		}
	};

	struct Triangle
	{
		Point p, c1, c2;

		Point centerCircle()
		{
			Point center;
			return center;
		}

		bool isDoublon(Triangle t)
		{
			//make si tout les points d'un triangle sont diffferents
			if (!t.p.equals2D(p) && !t.p.equals2D(c1) && !t.p.equals2D(c2))
				return false;
			if (!t.c1.equals2D(p) && !t.c1.equals2D(c1) && !t.c1.equals2D(c2))
				return false;
			if (!t.c2.equals2D(p) && !t.c2.equals2D(c1) && !t.c2.equals2D(c2))
				return false;
			return true;
		}
	};

	struct Segment
	{
		Point p1, p2;

		Point intersection(Segment s)
		{
			Point p;
			return p;
		}

		float coefDirecteur()
		{
			float coefDirecteur = -1;
			if (p1.x < p2.x)
			{
				return (p2.y - p1.y) / (p2.x - p1.x);
			}
			else if (p1.x < p2.x)
			{
				return (p2.y - p1.y) / (p2.x - p1.x);
			}
			return -1;
		}
	};
	
	typedef struct Point Point;

	struct Color
	{
		float r;
		float g;
		float b;
	};
	typedef struct Color Color;

	class Polygon
	{
		std::vector<maths::Point> *points;
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
		maths::Point calculateBezierPoints(double t);

		unsigned int g_num_cvs;
		unsigned int g_degree;
		unsigned int g_order;
		unsigned int g_num_knots;

	public:
		void setOutPolygon(maths::Polygon *pol);
		void setInPolygon(maths::Polygon *pol);
		maths::Polygon* getOutPolygon();
		maths::Polygon* getInPolygon();

		void calculateNormals();
		void calculateVectors();
		void addPoint(maths::Point p, int index);
		void addPoint(maths::Point p);
		maths::Point getPoint(int i);
		void removePoint();
		void removePoint(int index);
		void setVisibility(int index, bool visible);
		bool isPointVisible(int index);
		void setPoint(maths::Point p, int indice);
		const std::vector<maths::Point>* getNormals();
		Polygon();
		~Polygon();
		std::vector<maths::Point>* getPoints();
		std::vector<maths::Point>* getBezierPoints();
		void recalculateBezierPoints(int w, int h);
		void recalculateBezierPointsCoxDeBoor();
		void changeBezierRecursion(int nb);
		int factorial(int n);
		float binomial_coff(float n, float k);
	};
}


#endif // !MATHS_POLYGON