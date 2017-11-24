#include "Render.h"


Render* Render::instance = 0;

Render::Render(int screenWidth, int screenHeight)
{
	this->width = screenWidth;
	this->height = screenHeight;
}

Render::~Render(void)
{
}

void Render::animate()
{
	glViewport(0, 0, this->width, this->height);
	glClearColor(1.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto program = g_BasicShader.GetProgram();

	glUseProgram(program);

	auto color_location = glGetAttribLocation(program, "a_Color");
	auto position_location = glGetAttribLocation(program, "a_Position");

	//glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), &g_TriangleAttribs[2]);
	//glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), &g_TriangleAttribs[0]);

	//glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), g_Triangle);
	//glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), g_Color);

	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(color_location);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(position_location);
	glDisableVertexAttribArray(color_location);

	glUseProgram(0);

	glutSwapBuffers();
}

void LaunchAnimate()
{
	Render::instance->animate();
}

int Render::StartLoop(int argc, const char* argv)
{
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Triangle");

	#ifndef NO_GLEW
	glewInit();
	#endif

	g_BasicShader.LoadVertexShader("basic.vs");
	g_BasicShader.LoadFragmentShader("basic.fs");
	g_BasicShader.CreateProgram();

	glutDisplayFunc(LaunchAnimate);
	glutMainLoop();

	g_BasicShader.DestroyProgram();

	return 1;
}