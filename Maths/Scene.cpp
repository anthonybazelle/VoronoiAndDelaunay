#include "Scene.h"

using namespace maths;

Scene* Scene::currentInstance = nullptr;

maths::Point p0;

void Scene::drawCallBack()
{
	Scene::currentInstance->mainLoop();
}

void Scene::flush()
{
	if (state != ENTER_POINTS_POLYGON)
	{
		polygon.removePoint();
		glutPostRedisplay();
	}
}

void Scene::moveSelectedPoint(float x, float y)
{
	Point p;
	p.x = x;
	p.y = y;
	polygon.setPoint(p, pointSelected);
	glutPostRedisplay();
}


void Scene::selectPolygon(float x, float y)
{
	if (state == DRAW || RUN_JARVIS_MARCH || DELAUNAY)
	{
		maths::Point p;
		p.x = x;
		p.y = y;
		if (isPointInPol(polygon, p))
		{
			polygonSelected = true;
			glutPostRedisplay();
			return;
		}
	}
	polygonSelected = false;
	glutPostRedisplay();

}

bool Scene::hasSelectedPoint()
{
	return (polygonSelected != -1 && pointSelected != -1);
}


bool Scene::hasSelectedPolygon()
{
	return (polygonSelected != -1);
}

void Scene::lauchOpenGLLoop()
{
	glutMainLoop();
}

void Scene::initOpenGl(int argc, const char* argv)
{
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(width, height);

	windowId = glutCreateWindow("Maths");

	createMenu();

#ifndef NO_GLEW
	glewInit();
#endif

	g_BasicShader.LoadVertexShader("basic.vs");
	g_BasicShader.LoadFragmentShader("basic.fs");
	g_BasicShader.CreateProgram();

	glutDisplayFunc(Scene::drawCallBack);

	//glEnable(GL_CULL_FACE);

	input->init();

}

void Scene::createMenu()
{
	mainMenu = glutCreateMenu(Scene::menuCallBack);

	glutAddMenuEntry("Exit", 0);
	glutAddMenuEntry("Add points                   A", 1);
	glutAddMenuEntry("End edition                  E", 2);
	glutAddMenuEntry("Activate translation         W", 3);
	glutAddMenuEntry("Activate rotation            X", 4);
	glutAddMenuEntry("Activate scale               C", 5);
	glutAddMenuEntry("Unactivate all               V", 6);
	glutAddMenuEntry("Run Jarvis March             J", 8);
	glutAddMenuEntry("Run Graham Scan              G", 9);
	glutAddMenuEntry("Run Delaunay                 H", 10);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// On traite ici le choix de l'utilisateur dans le menu contextuel
void Scene::menu(int num) {
	switch (num)
	{
	case 0:
		glutDestroyWindow(windowId);
		exit(0);
		break;
	case 1:
		input->checkKeyboardInputs('a', 0, 0);
		break;
	case 2:
		input->checkKeyboardInputs('e', 0, 0);
		break;
	case 3:
		input->checkKeyboardInputs('w', 0, 0);
		break;
	case 4:
		input->checkKeyboardInputs('x', 0, 0);
		break;
	case 5:
		input->checkKeyboardInputs('c', 0, 0);
		break;
	case 6:
		input->checkKeyboardInputs('v', 0, 0);
		break;
	case 8:
		input->checkKeyboardInputs('j', 0, 0);
		break;
	case 9:
		input->checkKeyboardInputs('g', 0, 0);
	case 10:
		input->checkKeyboardInputs('h', 0, 0);
	default:
		break;
	}

	glutPostRedisplay();
}

void Scene::applyTransformation(char key)
{
	if (polygonSelected && pointSelected == -1)
	{
		if (activeTransformation == ROTATION)
		{
			int coef = 1;
			if (key == 'd')
				coef = -coef;
			rotate_point(&polygon, coef*3.1416/180);
			glutPostRedisplay();

		}
		else if (activeTransformation == SCALE)
		{
			float coef = 1.1;
			if (key == 's')
				coef = 0.9;

			scalePoint(&polygon, coef);
			glutPostRedisplay();
		}
		else if (activeTransformation == TRANSLATION)
		{
			float coef = 10;
			float x = 0, y = 0;
			if (key == 'z')
				y = 1;
			else if(key == 'q')
				x = -1;
			else if (key == 's')
				y = -1;
			else if (key == 'd')
				x = 1;

			translatePoint(&polygon, x/width* coef, y/height* coef);
			glutPostRedisplay();
		}
	}
}

void Scene::RunJarvis(std::vector<maths::Point>& points, std::vector<maths::Point>& envelop)
{
	// On récupère le point le plus à gauche, s'il y en a plusieurs sur le même axe X on recupère celui qui est le plus bas sur cet axe
	int l = 0;
	for (int i = 1; i < points.size(); i++)
		if (points[i].x < points[l].x || points[i].x == points[l].x && points[i].y < points[l].y)
			l = i;

	
	// On boucle tant que l'on ne retombe pas sur le point de départ
	int p = l, q;
	do
	{
		// On ajoute le dernier point trouvé à l'enveloppe
		envelop.push_back(points[p]);

		// On recherche ici le prochain point de tel sorte qu'il forme avec le precedent segment l'angle minimum et restant dans le sens anti-horaire
		q = (p+1) % points.size();
		for (int i = 0; i < points.size(); i++)
		{
			if (Orientation(points[p], points[i], points[q]) == 2)
				q = i;
		}
		p = q;

	} while (p != l); 
	
	// Pour fermer l'enveloppe
	envelop.push_back(points[p]);
}

maths::Point nextToTop(std::stack<Point> &S)
{
	maths::Point p = S.top();
	S.pop();
	maths::Point res = S.top();
	S.push(p);
	return res;
}

void Swap(maths::Point &p1, maths::Point &p2)
{
	maths::Point temp = p1;
	p1 = p2;
	p2 = temp;
}

int Distance(Point p1, Point p2)
{
	return (p1.x - p2.x)*(p1.x - p2.x) +
		(p1.y - p2.y)*(p1.y - p2.y);
}

// Orientation du triplet ordonné (p, q, r).
// 0 = p, q et r colinéaires
// 1 = Sens horaire
// 2 = Sens anti-horaire
int Scene::Orientation(maths::Point p, maths::Point q, maths::Point r)
{
	float val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) 
		return 0;
	return (val > 0)? 1: 2;
}

int Scene::Compare(const void *vp1, const void *vp2)
{
	maths::Point *p1 = (maths::Point *)vp1;
	maths::Point *p2 = (maths::Point *)vp2;

	int o = Orientation(p0, *p1, *p2);
	if (o == 0)
		return (Distance(p0, *p2) >= Distance(p0, *p1))? -1 : 1;

	return (o == 2)? -1: 1;
}


void Scene::RunGrahamScan(std::vector<maths::Point> points, std::vector<maths::Point>& result)
{
	// Comme pour Jarvis, on recupere le point le plus en bas à gauche
	int ymin = points[0].y, min = 0;
	for (int i = 1; i < points.size(); i++)
	{
		int y = points[i].y;

		if ((y < ymin) || (ymin == y && points[i].x < points[min].x))
			ymin = points[i].y, min = i;
	}

	Swap(points[0], points[min]);

	p0 = points[0];

	// Ici on tri la liste des points par rapport à leur angle
	// Si les points sont colinéaires, on tri par rapport à celui qui est le plus loin du point référent p0
	qsort(&points[1], points.size()-1, sizeof(Point), Compare);

	int m = 1;

	// Reprend un peu l'idée du tri par extraction si jamais tu vois l'algo, comme pour Jarvis
	for (int i = 1; i < points.size(); i++)
	{
		while (i < (points.size() - 1) && Orientation(p0, points[i], points[i+1]) == 0)
			i++;

		points[m] = points[i];
		m++;  
	}

	// minimum 3 points comme pour Jarvis sinon inutile
	if (m < 3) return;

	// Stack ou vector peu importe, à modifier ensuite p-e, mais pratique avec la stack
	std::stack<maths::Point> stack;
	stack.push(points[0]);
	stack.push(points[1]);
	stack.push(points[2]);

	for (int i = 3; i < m; i++)
	{
		// Dans le cas où l'on n'est pas dans le sens anti-horaire sur l'un des trois points, on depile
		while (Orientation(nextToTop(stack), stack.top(), points[i]) != 2)
			stack.pop();
		stack.push(points[i]);
	}

	int sizeStack = stack.size();

	// Pour fermer l'enveloppe, penser à push le point référent
	result.push_back(p0);

	for(int i=0; i< sizeStack; ++i)
	{
		//maths::Point p = stack.top();
		result.push_back(stack.top());
		stack.pop();
	}

	
}


void Scene::changeActiveTransformation(Transformation trans)
{
	activeTransformation = trans;
}




bool Scene::isPointSelected(float mX, float mY)
{
	if (state == DRAW || state==RUN_JARVIS_MARCH || DELAUNAY)
	{
		float nb = 10;
		float nbX = nb /width;
		float nbY = nb / height;

		std::cout << "mx = " << mX <<"  mY=" << mY << std::endl;
		for (int j = 0; j < polygon.getPoints()->size(); j++) 
		{
			maths::Point p = polygon.getPoints()->at(j);

			std::cout << "x=" << p.x << "   y=" << p.y << std::endl;
			if (mX > p.x - nbX && mX<p.x + nbX && mY>p.y - nbY && mY < p.y + nbY)
			{
				pointSelected = j;
				polygonSelected = true;
				return true;
			}
		}
	}
	pointSelected = -1;
	polygonSelected = false;

	return false;
}


void Scene::unselectPoint()
{
	pointSelected = -1;
	polygonSelected = -1;
}

float Scene::getWidth()
{
	return width;
}

float Scene::getHeight()
{
	return height;
}

void Scene::mainLoop()
{
	glViewport(0, 0, width, height);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);


	auto program = g_BasicShader.GetProgram();
	glUseProgram(program);

	auto color_position = glGetAttribLocation(program, "a_Color");
	auto position_location = glGetAttribLocation(program, "a_Position");


	GLuint colorID = glGetUniformLocation(program, "myColor");
	glUniform4f(colorID, color[0], color[1], color[2], color[3]);


	if (state == DRAW)
	{
		if (!polygon.getPoints()->empty())
		{
			if (polygonSelected)
			{
				glUniform4f(colorID, 1.0,0.0,0.0,1.0);
			}
			else
			{
				glUniform4f(colorID, color[0], color[1], color[2], color[3]);
			}

			if (pointSelected != -1 && polygonSelected)
			{
				const maths::Point *points = polygon.getPoints()->data();
				unsigned int size = pointSelected;

				glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
				glEnableVertexAttribArray(position_location);
				glPointSize(5);
				glDrawArrays(GL_POINTS, 0, size);
				glDisableVertexAttribArray(position_location);

				glUniform4f(colorID, 0.0, 1.0, 0.0, 1.0);
				size = 1;

				glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, &points[pointSelected]);
				glEnableVertexAttribArray(position_location);
				glPointSize(5);
				glDrawArrays(GL_POINTS, 0, size);
				glDisableVertexAttribArray(position_location);

				glUniform4f(colorID, 1.0, 0.0, 0.0, 1.0);
				size = polygon.getPoints()->size()- pointSelected-1;

				if (size > 0)
				{
					glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, &points[pointSelected+1]);
					glEnableVertexAttribArray(position_location);
					glPointSize(5);
					glDrawArrays(GL_POINTS, 0, size);
					glDisableVertexAttribArray(position_location);
				}
			}
			else
			{
				const maths::Point *points = polygon.getPoints()->data();
				unsigned int size = polygon.getPoints()->size();

				glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
				glEnableVertexAttribArray(position_location);

				glPointSize(5);

				glDrawArrays(GL_POINTS, 0, size);
				glDisableVertexAttribArray(position_location);
			}
				

		}

	}
	else if (state == ENTER_POINTS_POLYGON)
	{

		const maths::Point *points = polygon.getPoints()->data();
		unsigned int size = polygon.getPoints()->size();

		for (int i = 0; i < size; i++)
		{
			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, &points[i]);
			glEnableVertexAttribArray(position_location);

			glPointSize(10);

			glDrawArrays(GL_POINTS, 0, 1);
			glDisableVertexAttribArray(position_location);
		}
	}
	else if(state == RUN_JARVIS_MARCH)
	{
		if(polygon.getPoints()->empty())
		{
			std::cerr << "You forgot to add some points for the Jarvis march algorithm !" << std::endl;
		}
		else
		{
			// redondance ici a cause des else if
			const maths::Point *points = this->polygon.getPoints()->data();

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, this->polygon.getPoints()->size());
			glDisableVertexAttribArray(position_location);

			this->jarvisPoints->clear();
			RunJarvis(*(this->polygon.getPoints()), *(this->jarvisPoints));
			//ConvexHull(*(this->dataPointsJarvis), (*this->jarvisPoints));
			//DrawJarvisPolygon(*(this->dataPointsJarvis), (*this->jarvisPoints));

			if(!this->jarvisPoints->empty())
			{
				glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, this->jarvisPoints->data());
				glEnableVertexAttribArray(position_location);

				glPointSize(5);

				glDrawArrays(GL_LINE_STRIP, 0, this->jarvisPoints->size());
				glDisableVertexAttribArray(position_location);
			}
		}
	}
	else if (state == DELAUNAY)
	{
		if (polygon.getPoints()->size()<2)
		{
			std::cerr << "You forgot to add some points for the Delaunay algorithm !" << std::endl;
		}
		else
		{
			// redondance ici a cause des else if
			const maths::Point *points = this->polygon.getPoints()->data();

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, this->polygon.getPoints()->size());
			glDisableVertexAttribArray(position_location);

			triangularisation();
			
			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, delaunayLines->data());
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_LINES, 0, delaunayLines->size()*2);
			glDisableVertexAttribArray(position_location);
		}
	}
	else if(state == RUN_GRAHAM_SCAN)
	{
		const maths::Point *points = this->polygon.getPoints()->data();

		glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
		glEnableVertexAttribArray(position_location);

		glPointSize(5);

		glDrawArrays(GL_POINTS, 0, this->polygon.getPoints()->size());
		glDisableVertexAttribArray(position_location);

		if(this->polygon.getPoints()->size() > 3)
		{
			this->RunGrahamScan(*(this->polygon.getPoints()), *(this->grahamScanPoints));

			if(!this->grahamScanPoints->empty())
			{
				glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, this->grahamScanPoints->data());
				glEnableVertexAttribArray(position_location);

				glPointSize(5);

				glDrawArrays(GL_LINE_STRIP, 0, this->grahamScanPoints->size());
				glDisableVertexAttribArray(position_location);
			}
		}
	}

	glUseProgram(0);

	glutSwapBuffers();
}

void Scene::changeState(State s)
{
	if (state == s)
		return;
	state = s;
	/*switch (state)
	{
	case ENTER_POINTS_POLYGON:
		break;
	case DRAW:
		break;
	default:
		break;
	}*/
}

State Scene::getState()
{
	return state;
}

void Scene::addPoint(maths::Point p)
{
	switch (state)
	{
	case ENTER_POINTS_POLYGON:
			//std::cout << "point added x=" << p.x << " y=" << p.y << std::endl;
			p.y = height - p.y;
			p.x -= width / 2;
			p.y -= height / 2;
			p.x /= width / 2;
			p.y /= height / 2;
			//std::cout << "point normalized x=" << p.x << " y=" << p.y << std::endl;

			polygon.addPoint(p);

		break;
	case DRAW:
		break;
	default:
		break;
	}

}

void Scene::setDrawWindow()
{
	drawWindow = !drawWindow;
}

void Scene::menuCallBack(int i)
{
	Scene::currentInstance->menu(i);
}

void Scene::drawChar(const char c, const maths::Point position, const maths::Color color)
{
	//gluOrtho2D(0.0, width, 0.0, height);
	glColor3f(color.r, color.g, color.b);
	maths::Point p;
	p.x = (position.x*(width / 2)) + (width / 2);
	p.y= height -(position.y*(height/ 2)) + (height/ 2);

	glRasterPos2i(20, 20);
	void * font = GLUT_BITMAP_9_BY_15;
	glutBitmapCharacter(font, c);
}

maths::Point* Scene::isVertexFromPolygon(maths::Polygon polygon, maths::Point pointTest)
{
	for (int i = 0; i < polygon.getPoints()->size(); i++)
	{
		if (pointTest.x == polygon.getPoints()->at(i).x && pointTest.y == polygon.getPoints()->at(i).y)
		{
			return &(polygon.getPoints()->at(i));
		}
	}

	return nullptr;
}

maths::Point* Scene::ConvertPointPixelToOpenGLUnit(maths::Point point)
{
	maths::Point* pointOpenGL = new maths::Point();

	float v = 2.0 / (double)width;
	float w = 2.0 / (double)height;
	pointOpenGL->x = v * (double)point.x - 1.0;
	pointOpenGL->y = w * (double)point.y - 1.0;

	return pointOpenGL;
}

bool Scene::isPointInPol(maths::Polygon pol, maths::Point p)
{
	pol.calculateNormals();
	int nbPointWin = pol.getPoints()->size();

	for (int j = 0; j < nbPointWin; j++)
	{
		maths::Point p1 = pol.getPoints()->at(j);

		if (!Math::isPointVisible(p, p1, pol.getNormals()->at(j)))
			return false;
	}
	return true;
}

void Scene::cursorInPolygon(maths::Point p)
{
	bool dog = isPointInPol(polygon, p);
	std::cout << "test" << std::endl;
	if (dog == true)
	{
		stackPolygonClicked->push_back(polygon);
	}
}

void Scene::scalePoint(maths::Polygon *poly, float ratio)
{
	float pivotX = 0;
	float pivotY = 0;

	int nbPoints = poly->getPoints()->size();

	for (int i = 0; i < nbPoints; i++)
	{
		pivotX += poly->getPoints()->at(i).x;
		pivotY += poly->getPoints()->at(i).y;
	}

	pivotX = pivotX / nbPoints;
	pivotY = pivotY / nbPoints;

	// TODO : Test ratio

	for (int i = 0; i <poly->getPoints()->size(); i++)
	{
		Point p = poly->getPoints()->at(i);
		// application formule
		Point p2;
		p2.x = p.x- pivotX;
		p2.y = p.y - pivotY;

		p2.x *= ratio;
		p2.y *= ratio;

		p.x = pivotX + p2.x;
		p.y = pivotY + p2.y;

		poly->setPoint(p, i);
	}

}

void Scene::translatePoint(maths::Polygon *poly, float translateX, float translateY)
{
	for (int i = 0; i <poly->getPoints()->size(); i++)
	{
		Point p = poly->getPoints()->at(i);
		// application formule
		p.x = p.x + translateX;
		p.y = p.y + translateY;

		poly->setPoint(p, i);
	}
}

void Scene::rotate_point(maths::Polygon *poly,float angle)
{
	float s = sin(angle);
	float c = cos(angle);

	// On calcul la moyenne des coordonnées des sommets du polygon
	float pivotX = 0;
	float pivotY = 0;

	int nbPoints = poly->getPoints()->size();

	for (int i = 0; i < nbPoints ; i++)
	{
		pivotX += poly->getPoints()->at(i).x;
		pivotY += poly->getPoints()->at(i).y;
	}

	pivotX = pivotX / nbPoints;
	pivotY = pivotY / nbPoints;

	for (int i = 0; i <poly->getPoints()->size() ;i++ )
	{
		Point p = poly->getPoints()->at(i);
		// application formule
		p.x -= pivotX;
		p.y -= pivotY;

		// rotation du point
		float xnew = p.x * c - p.y * s;
		float ynew = p.x * s + p.y * c;

		p.x = xnew + pivotX;
		p.y = ynew + pivotY;

		poly->setPoint(p, i);
	}


}

void Scene::triangularisation()
{
	int size = polygon.getPoints()->size();

	delaunayLines->clear();

	for (int i = 0; i < size; i++)
	{
		Point p = polygon.getPoints()->at(i);
		for (int j = i+1 ; j < size; j++)
		{
			Line l = { p, polygon.getPoints()->at(j) };
			bool findIntersection = false;
			for (int k = 0; k < delaunayLines->size();k++)
			{
				findIntersection = l.hasIntersection(delaunayLines->at(k));
				if (findIntersection)
				{
					break;
				}
			}
			if (!findIntersection)
			{
				delaunayLines->push_back(l);
			}
		}
	}


}

Scene::Scene(int w, int h)
{
	state = DRAW;
	height = h;
	width = w;
	value = 0;
	drawWindow = true;
	isInPolygon = true;
	Scene::currentInstance = this;
	input = new Input(this);
	allIntersection = new std::vector<std::vector<maths::Point>*>();
	stackPolygonClicked = new std::vector<maths::Polygon>();

	this->jarvisPoints = new std::vector<maths::Point>();
	this->grahamScanPoints = new std::vector<maths::Point>();


	this->delaunayLines = new std::vector<maths::Line>();
	this->delaunayPoint = new std::vector<maths::Point>();

	srand(time(NULL));

	countPass = 0;
	radiusPoint.x = 10.0f/ width;
	radiusPoint.y = 10.0f /height;
	pointSelected = -1;
	polygonSelected = -1;
	color[0] = 0.0;
	color[1] = 0.0;
	color[2] = 1.0;
	color[3] = 1.0;
	activeTransformation = NO_TRANS;
}


Scene::~Scene()
{
	g_BasicShader.DestroyProgram();
}
