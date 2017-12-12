
#ifndef  DEF_SCENE
#define DEF_SCENE

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#include <Windows.h>
#endif
#include <iostream>
#ifdef NO_GLEW
#include <gl/GL.h>
#include "glext.h"
#else
#include "glew.h"
#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib")
#endif
#endif // !NO_GLEW

#include "freeglut.h"

#include "../common/EsgiShader.h"
#include "Input.h"

#include "Polygon.h"
#include "Math.h"
#include "Tools.h"
#include <map>
#include <vector>
#include <stack>
#include <time.h>
#include <stdlib.h> 
#include <stdio.h>
#include <chrono>

class Scene
{
	static Scene* currentInstance;
	maths::Point radiusPoint;
	static void drawCallBack();
	Input *input;
	EsgiShader g_BasicShader;
	int width;
	int height;
	void drawChar(const char c, const maths::Point position, const maths::Color color);
	bool drawWindow;
	void menu(int num);
	int windowId;
	int value;
	int option;
	int mainMenu;
	bool isInPolygon; 
	State state;
	std::vector<std::vector<maths::Point>*> *allIntersection ;
	std::vector<maths::Polygon> *stackPolygonClicked ;
	Tools tools;

	bool polygonSelected;
	int pointSelected;
	Transformation activeTransformation;

	// TEST ANTHO
	int countPass;
	maths::Polygon savePoly;
	

	// Jarvis
	std::vector<maths::Point>* jarvisPoints;

	// Graham Scan
	std::vector<maths::Point>* grahamScanPoints;


	void RunJarvis(std::vector<maths::Point>& points, std::vector<maths::Point>& envelop);

	void RunGrahamScan(std::vector<maths::Point> points, std::vector<maths::Point>& result);

	float color[4];

public:
	void flush();
	void lauchOpenGLLoop();
	//static void menuCallBack(int num);
	void initOpenGl(int argc, const char* argv);
	void mainLoop();
	void createMenu();
	maths::Point* isVertexFromPolygon(maths::Polygon polygon, maths::Point pointTest);
	maths::Point* ConvertPointPixelToOpenGLUnit(maths::Point point);
	void changeState(State state);
	State getState();
	void addPoint(maths::Point *p);
	void setDrawWindow();
	static void menuCallBack(int i);
	void rotate_point(maths::Polygon *poly, float angle);
	void scalePoint(maths::Polygon *poly, float ratio);
	void translatePoint(maths::Polygon *poly, float translateX, float translateY);
	Scene(int w, int h);
	~Scene();
	bool isPointSelected(float mX, float mY);
	float getWidth();
	float getHeight();
	void unselectPoint(); 
	void moveSelectedPoint(float x, float y);
	bool hasSelectedPoint();
	bool hasSelectedPolygon();
	void changeActiveTransformation(Transformation trans);
	Tools* getTools();
	void updatePoints();

	void removePoint(maths::Point* p);
	
	
	static int Orientation(maths::Point* p, maths::Point* q, maths::Point* r);
	static int Compare(const void *vp1, const void *vp2);

	void RunJarvis();
	void RunGraham();
};

#endif // ! SCENE






