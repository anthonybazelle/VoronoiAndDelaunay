#include <math.h>
#include <vector>
#include "Cam.hpp"
#include "Input.hpp"
#include "EnveloppeConv.hpp"
#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#include <windows.h>
#endif
#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib")
#endif
#include "../common/EsgiShader.h"
#include "../common/mat4.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
 
int sizetab, sizeind;
bool firstInit = true;
EsgiShader g_BasicShader;
std::vector<Point> p3D, tmpVectorPoints;           
float* tabPoints, *tmpPoints;         
GLushort* createInd(int);
GLushort* indi,*indTmp;			
GLuint VAO;
GLuint myVBO0;	
GLuint myVBO1;	
GLuint IBO,IBO1;	
GLuint TexObj; 
float couleur[4];
std::vector<Face*> *tmpFace = new std::vector<Face*>();
std::vector<Couleur> col;


bool Initialize()
{	couleur[0] = 0.0;
	couleur[1] = 1.0;
	couleur[2] = 1.0;
	couleur[3] = 1.0;
	std::vector<Point> centerPoints3D = createRandomPoints(10);
	Graph * tmpGraph = new Graph();
	EnveloppeConvexe testEnv = *new EnveloppeConvexe(tmpGraph, centerPoints3D);
	testEnv.initializeGraph();
	testEnv.algo();
	tmpFace = testEnv.getGraph()->getFaceList();
	tmpVectorPoints.clear();
	for (int i = 0; i < tmpFace->size(); i++)
	{		tmpVectorPoints.push_back(tmpFace->at(i)->getPoints()[0]);
		col.push_back(tmpFace->at(i)->getColor());
		tmpVectorPoints.push_back(tmpFace->at(i)->getPoints()[1]);
		col.push_back(tmpFace->at(i)->getColor());
		tmpVectorPoints.push_back(tmpFace->at(i)->getPoints()[2]);
		col.push_back(tmpFace->at(i)->getColor());
	}
	std::vector<Couleur> tmpColore;
	p3D = transformPointsToCube(centerPoints3D);
	tmpColore.clear();
	for (int i = 0; i < p3D.size(); i++)
	{		tmpColore.push_back(Couleur(red));
	}
	tabPoints = structToColorArray(p3D, tmpColore);
	indi = createInd(centerPoints3D.size() * 24);
	indTmp = createInd(tmpVectorPoints.size());
	tmpPoints = structToColorArray(tmpVectorPoints, col);
	glewInit();
	g_BasicShader.LoadVertexShader("basic.vs");
	g_BasicShader.LoadFragmentShader("basic.fs");
	g_BasicShader.CreateProgram();
	glGenTextures(1, &TexObj);
	glBindTexture(GL_TEXTURE_2D, TexObj);
	int w, h, c;
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenVertexArrays(1, &myVBO0);
	glBindVertexArray(myVBO0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO0);
	glBufferData(GL_ARRAY_BUFFER, p3D.size() * 9 * sizeof(float), tabPoints, GL_STATIC_DRAW);
	glGenVertexArrays(1, &myVBO1);
	glBindVertexArray(myVBO1);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO1);
	glBufferData(GL_ARRAY_BUFFER, tmpVectorPoints.size() * 9 * sizeof(float), tmpPoints, GL_STATIC_DRAW);
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p3D.size() * sizeof(GLushort), indi, GL_STATIC_DRAW);
	glGenBuffers(1, &IBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tmpVectorPoints.size() * sizeof(GLushort), indTmp, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (firstInit)
	{		ChangeCam(CamType);
		firstInit = false;
	}
	return true;}
int main(int argc, const char* argv[])
{	
	glutInit(&argc, (char**)argv);
	
	
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowPosition(100, 10);
	glutInitWindowSize(800, 800);
	glutCreateWindow("EnvellopeConv3D");
	gluOrtho2D(-400, 400, -400, 400);						
															
	glClearColor(1.0, 1.0, 1.0, 0.5);
	glColor3f(1.0, 1.0, 1.0);			     	 
	glPointSize(2.0);
#ifdef FREEGLUT
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
#ifdef NO_GLEW
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)
		wglGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)
		wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)
		wglGetProcAddress("glDisableVertexAttribArray");
#else
	glewInit();
#endif
	Initialize();
	glutIdleFunc(update);
	glutDisplayFunc(animate);
	glutPassiveMotionFunc(mouse);
	glutSpecialFunc(SpecialInput);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	Close();
	return 1;}
GLushort* createInd(int n)
{	GLushort* tmp = new GLushort[n];
	for (int i = 0; i < n ; i++)
	{		tmp[i] = i;
	}
	return tmp;}


float * structToColorArray(std::vector<Point> newPoints, std::vector<Couleur> c)
{
	float* tabP = new float[newPoints.size() * 9];
	int j = 0;
	for (int i = 0; i < newPoints.size() * 9; i += 9)
	{
		tabP[i] = newPoints[j].x;
		tabP[i + 1] = newPoints[j].y;
		tabP[i + 2] = newPoints[j].z;
		tabP[i + 3] = newPoints[j].n1;
		tabP[i + 4] = newPoints[j].n2;
		tabP[i + 5] = newPoints[j].n3;
		tabP[i + 6] = 0;
		tabP[i + 7] = 1;
		tabP[i + 8] = 0;
		j++;
	}
	return tabP;
}
void Close()
{
	glDeleteTextures(1, &TexObj);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &myVBO0);
	glDeleteBuffers(1, &IBO1);
	glDeleteBuffers(1, &myVBO1);
	g_BasicShader.DestroyProgram();
}
void update()
{
	glutPostRedisplay();
}
void animate()
{
	if (mustReinit)
	{
		mustReinit = false;
		Initialize();
	}


	TimeSinceAppStartedInMS = glutGet(GLUT_ELAPSED_TIME);
	TimeInSeconds = TimeSinceAppStartedInMS / 1000.0f;
	DeltaTime = (TimeSinceAppStartedInMS - OldTime) / 1000.0f;
	OldTime = TimeSinceAppStartedInMS;
	glViewport(0, 0, width, height);
	glClearColor(1.0, 1.0, 1.0, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	auto program = g_BasicShader.GetProgram();
	glUseProgram(program);
	Esgi::Mat4 worldMatrix;
	worldMatrix.MakeScale(1.0f, 1.0f, 1.0f);

	Esgi::Mat4 matriceCamera;
	switch (CamType)
	{
	case 0:
		matriceCamera = FPSCamera(posX, posY, posZ, rotX, rotY);
		break;
	case 1:
		matriceCamera = OrbitCamera(posX, posY, posZ, distance, rotX, rotY);
		break;
	}

	auto world_location = glGetUniformLocation(program, "u_WorldMatrix");
	glUniformMatrix4fv(world_location, 1, GL_FALSE, worldMatrix.m);
	Esgi::Mat4 projectionMatrix;
	float w = glutGet(GLUT_WINDOW_WIDTH), h = glutGet(GLUT_WINDOW_HEIGHT);

	float aspectRatio = w / h;
	float fovy = 45.0f;
	float nearZ = 0.1f;
	float farZ = 10000.0f;
	projectionMatrix.Perspective(fovy, aspectRatio, nearZ, farZ);

	auto projection_location = glGetUniformLocation(program, "u_ProjectionMatrix");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, projectionMatrix.m);
	auto camera_location = glGetUniformLocation(program, "u_CameraMatrix");
	glUniformMatrix4fv(camera_location, 1, GL_FALSE, matriceCamera.m);
	auto time_location = glGetUniformLocation(program, "u_Time");
	glUniform1f(time_location, TimeInSeconds);

	auto normal_location = glGetAttribLocation(program, "a_Normal");
	auto position_location = glGetAttribLocation(program, "a_Position");
	auto color_location = glGetAttribLocation(program, "a_Color");




	glBindBuffer(GL_ARRAY_BUFFER, myVBO0);

	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<const void *>(0 * sizeof(float)));
	glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<const void *>(3 * sizeof(float)));
	glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<const void *>(6 * sizeof(float)));



	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(normal_location);
	glEnableVertexAttribArray(color_location);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_QUADS, p3D.size(), GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, myVBO1);

	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<const void *>(0 * sizeof(float)));
	glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<const void *>(3 * sizeof(float)));
	glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<const void *>(6 * sizeof(float)));



	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(normal_location);
	glEnableVertexAttribArray(color_location);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
	glDrawElements(GL_TRIANGLES, tmpVectorPoints.size(), GL_UNSIGNED_SHORT, nullptr);


	glDisableVertexAttribArray(position_location);
	glDisableVertexAttribArray(normal_location);
	glDisableVertexAttribArray(color_location);

	glUseProgram(0);



	glEnd();
	glutSwapBuffers();
}