#pragma once

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#include <Windows.h>
#endif

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
#include "EsgiShader.h"


class Render
{
private:
	int width;
	int height;
	EsgiShader g_BasicShader;
	static const float* g_Triangle;
	static const float* g_Color;
	static const float* g_TriangleAttribs;

public:
	static Render* instance;

	Render(int screenWidth, int screenHeight);
	~Render(void);

	void animate();
	int StartLoop(int argc, const char* argv);
};

