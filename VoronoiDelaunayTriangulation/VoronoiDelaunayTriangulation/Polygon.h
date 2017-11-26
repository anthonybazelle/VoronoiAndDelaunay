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