#pragma once
#include "Sommet.hpp"

class Graph {
private:
	std::vector<Sommet*> *summitList;
	std::vector<Cote*> *edgeList;
	std::vector<Face*> *faceList;


public:
	Graph() {
		summitList = new std::vector<Sommet*>;
		edgeList = new std::vector<Cote*>;
		faceList = new std::vector<Face*>;
	}
	Graph(std::vector<Sommet*> *s, std::vector<Face*>* f, std::vector<Cote*>* e)
	{
		summitList = s;
		faceList = f;
		edgeList = e;
	}
	void setSummitList(std::vector<Sommet*>* s)
	{
		summitList = s;
	}
	std::vector<Sommet*> * getSummitList()
	{
		return summitList;
	}
	void setFaceList(std::vector<Face*> * f)
	{
		faceList = f;
	}
	std::vector<Face*> * getFaceList()
	{
		return faceList;
	}
	void setEdgeList(std::vector<Cote*>* e)
	{
		edgeList = e;
	}
	std::vector<Cote*> * getEdgeList()
	{
		return edgeList;
	}


	void visualiseGraph()
	{
		for (int i = 0; i < summitList->size(); i++)
		{
			std::cout << "Summit : " << std::endl;
			std::cout << summitList->at(i)->getPoint().toString() << std::endl;
			std::cout << summitList->at(i)->getColor() << std::endl;
			for (int j = 0; j < summitList->at(i)->getEdgesConnected()->size(); j++)
			{
				std::cout << "Edges : " << std::endl;
				std::cout << summitList->at(i)->getEdgesConnected()->at(j)->getPoints()[0].toString() << std::endl;
				std::cout << summitList->at(i)->getEdgesConnected()->at(j)->getPoints()[1].toString() << std::endl;
				std::cout << "Faces : " << std::endl;
				for (int k = 0; k < 3; k++)
				{
					std::cout << summitList->at(i)->getEdgesConnected()->at(j)->getFacesConnected()->at(0)->getPoints()[k].toString() << " ";
				}
				std::cout << "" << std::endl;
				std::cout << "Faces 1: " << std::endl;
				for (int k = 0; k < 3; k++)
				{
					std::cout << summitList->at(i)->getEdgesConnected()->at(j)->getFacesConnected()->at(1)->getPoints()[k].toString() << " ";
				}
				std::cout << "" << std::endl;

			}
		}
	}
};