#include "Scene.h"

using namespace maths;

Scene* Scene::currentInstance = nullptr;

maths::Point* p0;

void Scene::drawCallBack()
{
	Scene::currentInstance->mainLoop();
}

void Scene::flush()
{
	
}

void Scene::moveSelectedPoint(float x, float y)
{
	Point p = { x,y };
	tools.points[pointSelected] = {x,y};
	updatePoints();
	glutPostRedisplay();
}

bool Scene::hasSelectedPoint()
{
	return (polygonSelected ==true && pointSelected != -1);
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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
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

	glutAddMenuEntry("Exit            0", 0);
	glutAddMenuEntry("Points          1", 1);
	glutAddMenuEntry("Polygone        2", 2);
	glutAddMenuEntry("Jarvis          3", 5);
	glutAddMenuEntry("Graham          4", 4);
	glutAddMenuEntry("Triangulation   5", 5);
	glutAddMenuEntry("Delaunay        6", 6);
	glutAddMenuEntry("Voronoi         7", 7);

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
		input->checkKeyboardInputs('1', 0, 0);
		break;
	case 2:
		input->checkKeyboardInputs('2', 0, 0);
		break;
	case 3:
		input->checkKeyboardInputs('3', 0, 0);
		break;
	case 4:
		input->checkKeyboardInputs('4', 0, 0);
		break;
	case 5:
		input->checkKeyboardInputs('5', 0, 0);
		break;
	case 6:
		input->checkKeyboardInputs('6', 0, 0);
		break;
	case 7:
		input->checkKeyboardInputs('7', 0, 0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void Scene::RunJarvis()
{
	this->jarvisPoints->clear();
	RunJarvis(tools.points, *jarvisPoints);
}

void Scene::RunGraham()
{
	this->RunGrahamScan(tools.points, *(this->grahamScanPoints));
}


Tools* Scene::getTools()
{
	return &tools;
}

void Scene::RunJarvis(std::vector<maths::Point>& points, std::vector<maths::Point>& envelop)
{
	if (points.empty())
		return;
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
			if (Orientation(&points[p], &points[i], &points[q]) == 2)
				q = i;
		}
		p = q;

	} while (p != l); 
	
	// Pour fermer l'enveloppe
	envelop.push_back(points[p]);
}

maths::Point* nextToTop(std::stack<Point*> &S)
{
	maths::Point *p = S.top();
	S.pop();
	maths::Point *res = S.top();
	S.push(p);
	return res;
}

void Swap(maths::Point &p1, maths::Point &p2)
{
	maths::Point temp = p1;
	p1 = p2;
	p2 = temp;
}

int Distance(Point* p1, Point* p2)
{
	return (p1->x - p2->x)*(p1->x - p2->x) +
		(p1->y - p2->y)*(p1->y - p2->y);
}

// Orientation du triplet ordonné (p, q, r).
// 0 = p, q et r colinéaires
// 1 = Sens horaire
// 2 = Sens anti-horaire
int Scene::Orientation(maths::Point* p, maths::Point* q, maths::Point* r)
{
	float val = (q->y - p->y) * (r->x - q->x) -
		(q->x - p->x) * (r->y - q->y);

	if (val == 0) 
		return 0;
	return (val > 0)? 1: 2;
}

int Scene::Compare(const void *vp1, const void *vp2)
{
	maths::Point *p1 = (maths::Point *)vp1;
	maths::Point *p2 = (maths::Point *)vp2;

	int o = Orientation(p0, p1, p2);
	if (o == 0)
		return (Distance(p0, p2) >= Distance(p0, p1))? -1 : 1;

	return (o == 2)? -1: 1;
}


void Scene::RunGrahamScan(std::vector<maths::Point> points, std::vector<maths::Point>& result)
{
	if (points.size()<3)
		return;
	// Comme pour Jarvis, on recupere le point le plus en bas à gauche
	int ymin = points[0].y, min = 0;
	for (int i = 1; i < points.size(); i++)
	{
		int y = points[i].y;

		if ((y < ymin) || (ymin == y && points[i].x < points[min].x))
			ymin = points[i].y, min = i;
	}

	Swap(points[0], points[min]);

	p0 = &points[0];

	// Ici on tri la liste des points par rapport à leur angle
	// Si les points sont colinéaires, on tri par rapport à celui qui est le plus loin du point référent p0
	qsort(&points[1], points.size()-1, sizeof(Point), Compare);

	int m = 1;

	// Reprend un peu l'idée du tri par extraction si jamais tu vois l'algo, comme pour Jarvis
	for (int i = 1; i < points.size(); i++)
	{
		while (i < (points.size() - 1) && Orientation(p0, &points[i], &points[i+1]) == 0)
			i++;

		points[m] = points[i];
		m++;  
	}

	// minimum 3 points comme pour Jarvis sinon inutile
	if (m < 3) return;

	// Stack ou vector peu importe, à modifier ensuite p-e, mais pratique avec la stack
	std::stack<maths::Point*> stack;
	stack.push(&points[0]);
	stack.push(&points[1]);
	stack.push(&points[2]);

	for (int i = 3; i < m; i++)
	{
		// Dans le cas où l'on n'est pas dans le sens anti-horaire sur l'un des trois points, on depile
		while (Orientation(nextToTop(stack), stack.top(), &points[i]) != 2)
			stack.pop();
		stack.push(&points[i]);
	}

	int sizeStack = stack.size();

	// Pour fermer l'enveloppe, penser à push le point référent
	result.push_back(*p0);

	for(int i=0; i< sizeStack; ++i)
	{
		//maths::Point p = stack.top();
		result.push_back(*stack.top());
		stack.pop();
	}

	
}


void Scene::changeActiveTransformation(Transformation trans)
{
	activeTransformation = trans;
}




bool Scene::isPointSelected(float mX, float mY)
{
	float nb = 10;
	float nbX = nb / width;
	float nbY = nb / height;

	std::cout << "mx = " << mX << "  mY=" << mY << std::endl;
	for (int j = 0; j < tools.points.size(); j++)
	{
		maths::Point p = tools.points[j];

		std::cout << "x=" << p.x << "   y=" << p.y << std::endl;
		if (mX > p.x - nbX && mX<p.x + nbX && mY>p.y - nbY && mY < p.y + nbY)
		{
			pointSelected = j;
			polygonSelected = true;
			return true;
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
	


	if (state == POLYGONE)
	{
		color[0] = 0.0;
		color[1] = 0.0;
		color[2] = 0.0; 
		glUniform4f(colorID, color[0], color[1], color[2], color[3]);

		if (!tools.points.empty())
		{
			unsigned int size = tools.points.size();

			glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, tools.points.data());
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, size);
			glDisableVertexAttribArray(position_location);

			glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, tools.points.data());
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_LINE_LOOP, 0, size);
			glDisableVertexAttribArray(position_location);
		}

	}
	else if (state == VIEW_POINT)
	{
		color[0] = 0.0;
		color[1] = 0.0;
		color[2] = 0.0;
		glUniform4f(colorID, color[0], color[1], color[2], color[3]);

		unsigned int size = tools.points.size();

		glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, tools.points.data());
		glEnableVertexAttribArray(position_location);

		glPointSize(10);

		glDrawArrays(GL_POINTS, 0, size);
		glDisableVertexAttribArray(position_location);
	}
	else if(state == JARVIS)
	{
		color[0] = 0.0;
		color[1] = 1.0;
		color[2] = 0.0;
		glUniform4f(colorID, color[0], color[1], color[2], color[3]);

		if(tools.points.empty())
		{
			std::cerr << "You forgot to add some points for the Jarvis march algorithm !" << std::endl;
		}
		else
		{
			// redondance ici a cause des else if
			const maths::Point *points = tools.points.data();

			glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, tools.points.size());
			glDisableVertexAttribArray(position_location);


			if(!this->jarvisPoints->empty())
			{
				glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, this->jarvisPoints->data());
				glEnableVertexAttribArray(position_location);

				glPointSize(5);

				glDrawArrays(GL_LINE_STRIP, 0, this->jarvisPoints->size());
				glDisableVertexAttribArray(position_location);
			}
		}

	}
	else if (state == TRIANGULATION)
	{
		//delaunay qui marche
		if (tools.points.size()<2)
		{
			std::cerr << "You forgot to add some points for the Delaunay algorithm !" << std::endl;
		}
		else
		{
			color[0] = 0.0;
			color[1] = 0.0;
			color[2] = 1.0;
			glUniform4f(colorID, color[0], color[1], color[2], color[3]);

			unsigned int size = tools.cotes.size();

			Point *points = (Point*)malloc(size * 2 * sizeof(Point));
			int j = 0;
			for (int i = 0; i < size; i++)
			{
				points[j] = *tools.cotes[i].s1->p;
				points[j + 1] = *tools.cotes[i].s2->p;
				j += 2;
			}

			glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_LINES, 0, size * 2);
			glDisableVertexAttribArray(position_location);
		}


	}
	else if (state == DELAUNAY)
	{
		color[0] = 0.0;
		color[1] = 0.0;
		color[2] = 1.0;
		glUniform4f(colorID, color[0], color[1], color[2], color[3]);

		//delaunay qui marche
		if (tools.points.size()<2)
		{
			std::cerr << "You forgot to add some points for the Delaunay algorithm !" << std::endl;
		}
		else
		{

			unsigned int size = tools.cotes.size();

			Point *points = (Point*)malloc(size * 2 * sizeof(Point));
			int j = 0;
			for (int i = 0; i < size; i++)
			{
				points[j] = *tools.cotes[i].s1->p;
				points[j + 1] = *tools.cotes[i].s2->p;
				j += 2;
			}

			glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_LINES, 0, size * 2);
			glDisableVertexAttribArray(position_location);
		}


	}
	else if (state == VORONOI)
	{
		//delaunay qui marche
		if (tools.points.size()<2)
		{
			std::cerr << "You forgot to add some points for the Delaunay algorithm !" << std::endl;
		}
		else
		{
			color[0] = 0.0;
			color[1] = 0.0;
			color[2] = 0.0;
			glUniform4f(colorID, color[0], color[1], color[2], color[3]);

			unsigned int size = tools.points.size();

			glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, tools.points.data());
			glEnableVertexAttribArray(position_location);

			glPointSize(10);

			glDrawArrays(GL_POINTS, 0, size);
			glDisableVertexAttribArray(position_location);

			if (tools.points.size()>2)
			{
				color[0] = 1.0;
				color[1] = 0.0;
				color[2] = 0.0;
				glUniform4f(colorID, color[0], color[1], color[2], color[3]);

				float r, g, b;

				std::tie(r, g, b) = tools.voronoiColor;
				glColor3f(r, g, b);

				glBegin(GL_LINES);

				for (int i = 0; i < tools.sommets.size(); i++)
				{
					for (int k = 0; k < tools.regions.at(&tools.sommets.at(i)).size(); k++)
					{
						glVertex2f(tools.regions.at(&tools.sommets.at(i)).at(k).p1.x,
									tools.regions.at(&tools.sommets.at(i)).at(k).p1.y);
						glVertex2f(tools.regions.at(&tools.sommets.at(i)).at(k).p2.x,
							tools.regions.at(&tools.sommets.at(i)).at(k).p2.y);
					}
				}

				glEnd();
			}
		}


	}
	else if(state == GRAHAM)
	{
		color[0] = 0.0;
		color[1] = 1.0;
		color[2] = 0.0;
		std::cout << "Graham";
		const maths::Point *points = tools.points.data();

		glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, points);
		glEnableVertexAttribArray(position_location);

		glPointSize(5);

		glDrawArrays(GL_POINTS, 0, tools.points.size());
		glDisableVertexAttribArray(position_location);

		if(tools.points.size() > 3)
		{

			if(!this->grahamScanPoints->empty())
			{
				glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, this->grahamScanPoints->data());
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

void Scene::addPoint(maths::Point* p)
{
	p->y = height - p->y;
	p->x -= width / 2;
	p->y -= height / 2;
	p->x /= width / 2;
	p->y /= height / 2;
	//std::cout << "point normalized x=" << p.x << " y=" << p.y << std::endl;

	tools.points.push_back(*p);

	updatePoints();
}

void Scene::removePoint(Point* p)
{  
	p->y = height - p->y;
	p->x -= width / 2;
	p->y -= height / 2;
	p->x /= width / 2;
	p->y /= height / 2;

	float nb = 10;
	float nbX = nb / width;
	float nbY = nb / height;

	for (int j = 0; j < tools.points.size(); j++)
	{
		maths::Point p2 = tools.points[j];

		if (p->x > p2.x - nbX && p->x<p2.x + nbX && p->y>p2.y - nbY && p->y < p2.y + nbY)
		{
			tools.points.erase(tools.points.begin() + j);
			updatePoints();
			return;
		}
	}

}

void Scene::updatePoints()
{
	switch (state)
	{
	case JARVIS:
		RunJarvis();
		break;
	case GRAHAM:
		RunGraham();
		break;
	case TRIANGULATION:
		tools.triangulation();
		break;
	case DELAUNAY:
		tools.triangulation();
		tools.delaunayTriangulation();
		break;
	case VORONOI:
		tools.triangulation();
		tools.delaunayTriangulation();
		tools.voronoiAlgo();
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
		if (pointTest.x == polygon.getPoints()->at(i)->x && pointTest.y == polygon.getPoints()->at(i)->y)
		{
			return (polygon.getPoints()->at(i));
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

void Scene::scalePoint(maths::Polygon *poly, float ratio)
{
	float pivotX = 0;
	float pivotY = 0;

	int nbPoints = poly->getPoints()->size();

	for (int i = 0; i < nbPoints; i++)
	{
		pivotX += poly->getPoints()->at(i)->x;
		pivotY += poly->getPoints()->at(i)->y;
	}

	pivotX = pivotX / nbPoints;
	pivotY = pivotY / nbPoints;

	// TODO : Test ratio

	for (int i = 0; i <poly->getPoints()->size(); i++)
	{
		Point *p = poly->getPoints()->at(i);
		// application formule
		Point p2;
		p2.x = p->x- pivotX;
		p2.y = p->y - pivotY;

		p2.x *= ratio;
		p2.y *= ratio;

		p->x = pivotX + p2.x;
		p->y = pivotY + p2.y;

		poly->setPoint(p, i);
	}

}

void Scene::translatePoint(maths::Polygon *poly, float translateX, float translateY)
{
	for (int i = 0; i <poly->getPoints()->size(); i++)
	{
		Point *p = poly->getPoints()->at(i);
		// application formule
		p->x = p->x + translateX;
		p->y = p->y + translateY;

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
		pivotX += poly->getPoints()->at(i)->x;
		pivotY += poly->getPoints()->at(i)->y;
	}

	pivotX = pivotX / nbPoints;
	pivotY = pivotY / nbPoints;

	for (int i = 0; i <poly->getPoints()->size() ;i++ )
	{
		Point *p = poly->getPoints()->at(i);
		// application formule
		p->x -= pivotX;
		p->y -= pivotY;

		// rotation du point
		float xnew = p->x * c - p->y * s;
		float ynew = p->x * s + p->y * c;

		p->x = xnew + pivotX;
		p->y = ynew + pivotY;

		poly->setPoint(p, i);
	}


}

Scene::Scene(int w, int h)
{
	state = VIEW_POINT;
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
	tools = Tools();


}


Scene::~Scene()
{
	g_BasicShader.DestroyProgram();
}
