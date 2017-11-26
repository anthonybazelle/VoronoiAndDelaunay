#ifndef DEF_INPUT
#define DEF_INPUT


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


enum ButtonState {
	PRESSED, CLICKED, NONE
};

enum State {
	ENTER_POLYGON, ENTER_WINDOW, DRAW, FILL, COLOR
};

enum IntermediateState{
	NOCHOSEN, EXTRUDE, REVOLUTION, GENERALIZE
};

enum Transformation {
	TRANSLATION, ROTATION, SCALE, NO_TRANS
};

class Scene;

class Input
{

	static Input* currentInstance;
	ButtonState* mouseButtons;
	ButtonState* letters;
	Scene* scene;
	bool mouseMove;
	float mouseX;
	float mouseY;


	static void keyboardCallBack(unsigned char  touche, int x, int y);
	static void mouseCallBack(int button, int state, int x, int y);
	static void keyboardUpCallBack(unsigned char  touche, int x, int y);

public:
	Input(Scene *scene);
	~Input();
	void init();
	bool isLetterPressed(char letter);
	bool isLetterClicked(char letter);
	bool isMouseButtonPressed(int button);
	bool isMouseButtonClicked(int button);

	void checkKeyboardInputs(unsigned char  touche, int x, int y);
	void checkMouseClicks(int button, int state, int x, int y);
	void checkMouseMoves(int x, int y);
	static void mouseMoveCallBack(int x, int y);
	float getMouseX();
	float getMouseY();
	bool mouseHasMove();
	void SetMouseMove(bool b);
	void checkKeyboardUpInputs(unsigned char  touche, int x, int y);
};

#endif




