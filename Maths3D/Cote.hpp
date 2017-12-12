#pragma once
#include "Face.hpp"
#include <vector>
class Cote
{
private:

	std::vector<Face*> * faceConnected = new std::vector<Face*>();
	Couleur color;
	std::vector<Point> points;

public:
	Cote() {
	
		faceConnected = new std::vector<Face*>();
	}
	Cote(Couleur c, std::vector<Face*> *f)
	{
		color = c;
		faceConnected = f;
	}
	Cote(std::vector<Face*> *f, std::vector<Point> p)
	{
		faceConnected = f;
		points = p;
	}
	Cote(std::vector<Point> p)
	{
		faceConnected = new std::vector<Face*>();
		points = p;
	}
	void setColor(const Couleur & c)
	{
		color = c;
	}
	const Couleur & getColor() const
	{
		return color;
	}
	void setFacesConnected(std::vector<Face*> * f)
	{
		faceConnected = f;
	}
	std::vector<Face*> * getFacesConnected()
	{
		return faceConnected;
	}
	std::vector<Point>& getPoints()
	{
		return points;
	}
	void setPoints(const std::vector<Point> & p)
	{
		points = p;
	}

};