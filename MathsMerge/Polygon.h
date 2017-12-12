#ifndef DEF_MATHS_POLYGON
#define DEF_MATHS_POLYGON

#include <vector>
#include <iostream>

namespace maths
{


	struct Point;
	struct Sommet;
	struct Cote;
	struct Segment;
	struct Triangle;
	struct Cercle;


	

	typedef struct Point
	{
		float x;
		float y;
	} Point;


	typedef struct Sommet
	{
		Point* p;
		Cote* a;

		Sommet() : p(nullptr), a(nullptr) {};

		Sommet(Point* point) : p(point), a(nullptr) {};
	} Apex;

	

	typedef struct Cote
	{
		Sommet* s1;
		Sommet* s2;
		Triangle* td;
		Triangle* tg;

		Cote() : s1(nullptr), s2(nullptr), td(nullptr), tg(nullptr) {};

		Cote(Sommet* s1, Sommet* s2) : s1(s1), s2(s2), td(nullptr), tg(nullptr) {};
	} Edge;

	

	typedef struct Segment
	{
		Point p1;
		Point p2;

		Segment(Point p1, Point p2) : p1(p1), p2(p2) {};
	} Bounded_Edge;

	typedef struct Triangle
	{
		Cote* a1;
		Cote* a2;
		Cote* a3;

		Triangle() : a1(nullptr), a2(nullptr), a3(nullptr) {};

		Triangle(Cote* e1, Cote* e2, Cote* e3) : a1(e1), a2(e2), a3(e3) {};
	} Triangle;


	typedef struct Cercle
	{
		Point center;
		float radius;
	} Circle;

	struct points_comparator
	{
		inline bool operator() (const Point& p1, const Point& p2)
		{
			if (p1.x < p2.x) return true;
			if (p1.x > p2.x) return false;
			if (p1.y < p2.y) return true;
			return false;
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

	bool operator==(const Point& p1, const Point& p2);
	bool operator==(const Sommet& s1, const Sommet& s2);
	bool operator==(const Cote& a1, const Cote& a2);
	bool operator==(const Triangle& t1, const Triangle& t2);
}




#endif // !MATHS_POLYGON