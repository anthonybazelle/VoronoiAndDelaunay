

#include "Cote.hpp"
class Sommet {
	// each point connected to edges
private:
	Point point;

	std::vector<Cote*> * edgesConnected;
	Couleur color;

public:
	Sommet() {
		edgesConnected = new std::vector<Cote*>();
	}
	Sommet(Point p)
	{
		point = p;
		edgesConnected = new std::vector<Cote*>();
	}
	Sommet(Couleur c, Point p)
	{
		color = c;
		point = p;
	}

	Sommet(Couleur c, std::vector<Cote*> *e, Point p)
	{
		color = c;

		edgesConnected = e;
		point = p;
	}

	void setPoint(const Point & p)
	{
		point = p;
	}

	Point getPoint()
	{
		return point;
	}

	void setColor(const Couleur & c)
	{
		color = c;
	}

	const Couleur & getColor() const
	{
		return color;
	}

	void setEdgesConnected(std::vector<Cote*> * f)
	{
		edgesConnected = f;
	}

	std::vector<Cote*> * getEdgesConnected()
	{
		return edgesConnected;
	}

};