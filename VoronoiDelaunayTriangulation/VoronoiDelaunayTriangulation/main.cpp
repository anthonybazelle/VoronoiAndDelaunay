#include "Render.h"
#include <iostream>

int main(int argc, const char* argv)
{
	Render* pRender = new Render(600, 800);
	int result = pRender->StartLoop(argc, argv);
}