#include "Scene.h"

int main(int argc, const char* argv)
{
	Scene *scene = new Scene(1200, 800);

	scene->initOpenGl(argc, argv); 

	scene->lauchOpenGLLoop();

	return 1;
}


