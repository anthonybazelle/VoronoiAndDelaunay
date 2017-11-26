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
#include "glew/GL/glew.h"
#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib")
#endif
#endif // !NO_GLEW

#include "freeglut/GL/freeglut.h"

#include "CVecteur.h"
#include "EsgiShader.h"
#include "Input.h"

#include "Polygon.h"
#include "Math.h"
#include <map>
#include <vector>

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
	IntermediateState intermediateState;
	std::vector<maths::Polygon> *polygons; 
	maths::Polygon *window;
	std::vector<std::vector<maths::Point>*> *allIntersection ;
	std::vector<maths::Polygon> *stackPolygonClicked ;

	int polygonSelected;
	int pointSelected;
	Transformation activeTransformation;

	// TEST ANTHO
	int countPass;
	maths::Polygon savePoly;

	float color[4];

public:
	void flush();
	void lauchOpenGLLoop();
	//static void menuCallBack(int num);
	void initOpenGl(int argc, const char* argv);
	std::vector<maths::Point>* LCARemplissage(maths::Polygon polygon);
	void mainLoop();
	void createMenu();
	maths::Point* isVertexFromPolygon(maths::Polygon polygon, maths::Point pointTest);
	bool isPointInPol(maths::Polygon pol, maths::Point p);
	void cursorInPolygon(maths::Point p);
	maths::Point* ConvertPointPixelToOpenGLUnit(maths::Point point);
	void changeState(State state);
	void changeIntermediateState(IntermediateState s);
	State getState();
	void addPoint(maths::Point p);
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
	void selectPolygon(float x, float y);
	bool hasSelectedPoint();
	bool hasSelectedPolygon();
	void changeActiveTransformation(Transformation trans);
	void applyTransformation(char key);
	void linkOtherCurve();
	void changeBezierRecursion(int nb);
	std::vector<maths::Point>* SortPointForRevolution(std::vector<maths::Point>* revoPoint, int nbPointInCurve, int degree);
};

#endif // ! SCENE






