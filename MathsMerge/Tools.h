#pragma once
#include "Polygon.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm> 
#include <functional> 
#include <set> 
#define PI 3.14159265

class Tools
{
public:
	int maxPoints = 50;
	float colinearity_tolerance = 0.0001;
	bool voronoi;
	int menuPrincipal = 0;
	int colorSubMenu;
	int voronoiColorSubMenu;
	std::tuple<float, float, float> triangulationColor;
	std::tuple<float, float, float> voronoiColor;
	std::vector<maths::Point> points;
	std::vector<maths::Sommet> sommets;
	std::vector<maths::Cote> cotes;
	std::vector<maths::Triangle> triangles;
	std::map<maths::Sommet*, std::vector<maths::Segment>> regions;

	void triangulation();
	void delaunayTriangulation();
	void voronoiAlgo();
	bool isTriangleDirect(maths::Triangle);
	bool isTriangleLeft(maths::Triangle, maths::Edge*);
	float determinant(maths::Apex*, maths::Apex*, maths::Apex*);
	bool delaunayCriteria(maths::Edge*);
	std::vector<maths::Cote*> cotesAdjacents(maths::Point*);
	float orientedAngle(maths::Edge*, maths::Edge*, maths::Point*);
	std::vector<maths::Cote*> cotesOrienteAdjacents(maths::Point*);
	std::vector<maths::Triangle*> trianglesAdjacentsOrientes(std::vector<maths::Cote*>);
	std::vector<maths::Cote*> cotesNonAdjacents(std::vector<maths::Cote*>, std::vector<maths::Triangle*>);
	std::tuple<maths::Sommet*, maths::Sommet*, maths::Sommet*> sommetsTriangle(maths::Triangle*);
	maths::Cercle cercleCirconscrit(maths::Apex*, maths::Apex*, maths::Apex*);
	bool pointIsInTriangle(maths::Triangle*, maths::Apex*);
	std::vector<maths::Cote*> findSeenEdges(maths::Apex*);
	void addTriangleToEdges(maths::Triangle*);
	bool findEdge(maths::Apex*, maths::Apex*);
	Tools();
};


