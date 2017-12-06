#include "Scene.h"

int main(int argc, const char* argv)
{
	Scene *scene = new Scene(600, 600);

	scene->initOpenGl(argc, argv);

	scene->lauchOpenGLLoop();

	return 1;
}


