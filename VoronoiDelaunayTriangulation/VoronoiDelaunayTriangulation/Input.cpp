#include "Input.h"
#include "Scene.h"

Input* Input::currentInstance = nullptr;

void Input::keyboardCallBack(unsigned char touche, int x, int y)
{
	Input::currentInstance->checkKeyboardInputs(touche, x, y);
}

void Input::keyboardUpCallBack(unsigned char touche, int x, int y)
{
	Input::currentInstance->checkKeyboardUpInputs(touche, x, y);
}

void Input::mouseCallBack(int button, int state, int x, int y)
{
	Input::currentInstance->checkMouseClicks(button, state, x, y);
}

void Input::mouseMoveCallBack(int x, int y)
{
	Input::currentInstance->checkMouseMoves(x, y);
}

Input::Input(Scene *s)
{
	Input::currentInstance = this;
	mouseButtons = new ButtonState[3];
	for (int i = 0; i < 3; i++)
		mouseButtons[i] = NONE;
	letters = new ButtonState[26];
	for (int i = 0; i < 26; i++)
		letters[i] = NONE;
	scene = s;
}


Input::~Input()
{
}

void Input::init()
{
	glutKeyboardFunc(Input::keyboardCallBack);
	glutMouseFunc(Input::mouseCallBack);
	glutPassiveMotionFunc(Input::mouseMoveCallBack);
	glutKeyboardUpFunc(Input::keyboardUpCallBack);
}

bool Input::isLetterPressed(char letter)
{
	int nb = letter - (int)'a';
	if (nb<0 || nb>25)
		return false;
	return (letters[nb] == PRESSED);
}

bool Input::isLetterClicked(char letter)
{
	int nb = letter - (int)'a';
	if (nb<0 || nb>25)
		return false;
	if (letters[nb] == CLICKED)
	{
		letters[nb] = NONE;
		return true;
	}
	return false;
}

void Input::checkKeyboardUpInputs(unsigned char  touche, int x, int y)
{
	int nb = touche - (int)'a';
	if (nb<0 || nb>25)
		return;
	if (letters[nb] == CLICKED)
		letters[nb] = NONE;
	else
		letters[nb] = CLICKED;

}

bool Input::isMouseButtonPressed(int button)
{
	if (button<0 || button>2)
		return false;
	return (mouseButtons[button] == PRESSED);
}

bool Input::isMouseButtonClicked(int button)
{
	if (button<0 || button>2)
		return false;
	if (mouseButtons[button] == CLICKED)
	{
		mouseButtons[button] = NONE;
		return true;
	}
	return false;
}

void Input::checkKeyboardInputs(unsigned char  touche, int x, int y)
{
	std::cout << "Keypressed : " << (char)touche << std::endl;
	switch (touche)
	{
	case 'a':
		std::cout << "change state : ENTER_POINTS" << std::endl;
		scene->changeState(ENTER_POLYGON);
		glutPostRedisplay();
		break;
	case 'e':
		std::cout << "change state : DRAW" << std::endl;
		scene->changeState(DRAW);
		glutPostRedisplay();
		break;
	case 'w':
		std::cout << "ACTIVATE TRANSLATION" << std::endl;
		scene->changeActiveTransformation(TRANSLATION);
		glutPostRedisplay();
		break;
	case 'x':
		std::cout << "ACTIVATE ROTATION" << std::endl;
		scene->changeActiveTransformation(ROTATION);
		glutPostRedisplay();
		break;
	case 'c':
		std::cout << "ACTIVATE SCALE" << std::endl;
		scene->changeActiveTransformation(SCALE);
		glutPostRedisplay();
		break;
	case 'v':
		std::cout << "UNATIVE ALL" << std::endl;
		scene->changeActiveTransformation(NO_TRANS);
		glutPostRedisplay();
		break;
	case 'u':
		std::cout << "DRAW EXTRUDE" << std::endl;
		scene->changeIntermediateState(EXTRUDE);
		glutPostRedisplay();
		break;
	case 'r':
		std::cout << "DRAW REVOLUTION" << std::endl;
		scene->changeIntermediateState(REVOLUTION);
		glutPostRedisplay();
		break;
	case 'z':
		scene->applyTransformation(touche);
		glutPostRedisplay();
		break;
	case 'q':
		scene->applyTransformation(touche);
		glutPostRedisplay();
		break;
	case 's':
		scene->applyTransformation(touche);
		glutPostRedisplay();
		break;
	case 'd':
		scene->applyTransformation(touche);
		glutPostRedisplay();
		break;
	case 'l':
		std::cout << "link" << std::endl;
		scene->linkOtherCurve();
		glutPostRedisplay();
		break;
	case '+':
		scene->changeBezierRecursion(1);
		glutPostRedisplay();
		break;
	case '-':
		scene->changeBezierRecursion(-1);
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void Input::checkMouseClicks(int button, int state, int x, int y)
{
	State sceneState = scene->getState();
	if (sceneState == DRAW && button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseButtons[0] = PRESSED;
			if (scene->hasSelectedPoint())
			{
				scene->unselectPoint();
			}
			else
			{
				if (scene->isPointSelected(mouseX, mouseY))
				{
					std::cout << "point selected" << std::endl;
				}
			}
			

			glutPostRedisplay();
			std::cout << "mouseButton pressed"<<std::endl;
		}
		else
		{
			mouseButtons[0] = CLICKED;
			glutPostRedisplay();
			std::cout << "mouseButton up" << std::endl;
		}

	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && (sceneState == ENTER_POLYGON || sceneState == ENTER_WINDOW)) {
		maths::Point p;
		p.x = x;
		p.y = y;
		p.z = 0;
		scene->addPoint(p);
		//onMouse = 1;
		glutPostRedisplay();
	}

	/*if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && scene->getState() == DRAW)
	{
	maths::Point p;
	p.x = x;
	p.y = y;

	scene->cursorInPolygon(p);
	}*/
}

bool Input::mouseHasMove()
{
	if (mouseMove)
	{
		mouseMove = false;
		return true;
	}
}

void Input::SetMouseMove(bool b)
{
	mouseMove = b;
}

float Input::getMouseX()
{
	return mouseX;
}

float Input::getMouseY()
{
	return mouseY;
}


void Input::checkMouseMoves(int x, int y)
{

	mouseMove = true;

	float mX = x;
	float mY = y;
	
	float width = scene->getWidth();
	float height = scene->getHeight();

	mX -= width / 2;
	mX /= width / 2;
	mY = height - mY;
	mY -= height / 2;
	mY /= height / 2;
	mX = Math::round(mX);
	mY = Math::round(mY);

	mouseX = mX;
	mouseY = mY;

	if(scene->hasSelectedPoint())
		scene->moveSelectedPoint(mouseX, mouseY);
}