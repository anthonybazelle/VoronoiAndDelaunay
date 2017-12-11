#pragma once
#include "Point.hpp"
#include "Couleur.hpp"
#include <vector>
#include <iostream>

class Face {


private:
	Couleur color;
	std::vector<Point> points;

public:
	Face() {}
	Face(Couleur c, std::vector<Point> p)
	{
		color = c;

		points = p;
	}
	Face(std::vector<Point> p)
	{
		points = p;
	}
	const Couleur & getColor() const
	{
		return color;
	}
	void setColor(const Couleur & c)
	{
		color = c;
	}
	std::vector<Point> & getPoints()
	{
		return points;
	}
	void setPoints(const std::vector<Point> & p)
	{
		points = p;
	}
	void setPointInd(int ind,Point p)
	{
		points[ind] = p;
	}




};