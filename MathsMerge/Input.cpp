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
	deletePoint = false;
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
	case 'd':
		deletePoint = !deletePoint;
		break;
	case '1':
		std::cout << "change state : POINT" << std::endl;
		scene->changeState(VIEW_POINT);
		glutPostRedisplay();
		break;
	case '2':
		std::cout << "change state : POLYGONE" << std::endl;
		scene->changeState(POLYGONE);
		glutPostRedisplay();
		break;
	case '3':
		std::cout << "change state : JARVIS" << std::endl;
		scene->changeState(JARVIS);
		scene->RunJarvis();
		glutPostRedisplay();
		break;
	case '4':
		std::cout << "change state : GRAHAM" << std::endl;
		scene->changeState(GRAHAM);
		scene->RunGraham();
		glutPostRedisplay();
		break;
	case '5':
		std::cout << "change state : TRIANGULATION" << std::endl;
		scene->changeState(TRIANGULATION);
		scene->getTools()->triangulation();
		glutPostRedisplay();
		break;
	case '6':
		std::cout << "change state : DELAUNAY" << std::endl;
		scene->changeState(DELAUNAY);
		scene->getTools()->triangulation();
		scene->getTools()->delaunayTriangulation();
		glutPostRedisplay();
		break;
	case '7':
		std::cout << "change state : VORONOI" << std::endl;
		scene->changeState(VORONOI);
		scene->getTools()->triangulation();
		scene->getTools()->delaunayTriangulation();
		scene->getTools()->voronoiAlgo();
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void Input::checkMouseClicks(int button, int state, int x, int y)
{
	State sceneState = scene->getState();
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
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
		std::cout << "mouseButton pressed" << std::endl;
	}
	
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN && mouseButtons[0]!=PRESSED) {
		maths::Point *p = new maths::Point();
		p->x = x;
		p->y = y;
		if (deletePoint)
		{
			scene->removePoint(p);
		}
		else
		{
			scene->addPoint(p);
		}
		
		
		//onMouse = 1;
		glutPostRedisplay();
	}
	else if(state == GLUT_UP && button == GLUT_LEFT_BUTTON)
	{
		mouseButtons[0] = CLICKED;
		glutPostRedisplay();
		std::cout << "mouseButton up" << std::endl;
	}

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