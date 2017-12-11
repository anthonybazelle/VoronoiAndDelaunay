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
	if (state != ENTER_POINTS_POLYGON)
	{
		polygon.removePoint();
		glutPostRedisplay();
	}
}

void Scene::moveSelectedPoint(float x, float y)
{
	Point *p = new Point();
	p->x = x;
	p->y = y;
	polygon.setPoint(p, pointSelected);
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

void Scene::RunJarvis(std::vector<maths::Point*>& points, std::vector<maths::Point*>& envelop)
{
	// On r�cup�re le point le plus � gauche, s'il y en a plusieurs sur le m�me axe X on recup�re celui qui est le plus bas sur cet axe
	int l = 0;
	for (int i = 1; i < points.size(); i++)
		if (points[i]->x < points[l]->x || points[i]->x == points[l]->x && points[i]->y < points[l]->y)
			l = i;

	
	// On boucle tant que l'on ne retombe pas sur le point de d�part
	int p = l, q;
	do
	{
		// On ajoute le dernier point trouv� � l'enveloppe
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

// Orientation du triplet ordonn� (p, q, r).
// 0 = p, q et r colin�aires
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


void Scene::RunGrahamScan(std::vector<maths::Point*> points, std::vector<maths::Point*>& result)
{
	// Comme pour Jarvis, on recupere le point le plus en bas � gauche
	int ymin = points[0]->y, min = 0;
	for (int i = 1; i < points.size(); i++)
	{
		int y = points[i]->y;

		if ((y < ymin) || (ymin == y && points[i]->x < points[min]->x))
			ymin = points[i]->y, min = i;
	}

	Swap(*points[0], *points[min]);

	p0 = points[0];

	// Ici on tri la liste des points par rapport � leur angle
	// Si les points sont colin�aires, on tri par rapport � celui qui est le plus loin du point r�f�rent p0
	qsort(&points[1], points.size()-1, sizeof(Point), Compare);

	int m = 1;

	// Reprend un peu l'id�e du tri par extraction si jamais tu vois l'algo, comme pour Jarvis
	for (int i = 1; i < points.size(); i++)
	{
		while (i < (points.size() - 1) && Orientation(p0, points[i], points[i+1]) == 0)
			i++;

		points[m] = points[i];
		m++;  
	}

	// minimum 3 points comme pour Jarvis sinon inutile
	if (m < 3) return;

	// Stack ou vector peu importe, � modifier ensuite p-e, mais pratique avec la stack
	std::stack<maths::Point*> stack;
	stack.push(points[0]);
	stack.push(points[1]);
	stack.push(points[2]);

	for (int i = 3; i < m; i++)
	{
		// Dans le cas o� l'on n'est pas dans le sens anti-horaire sur l'un des trois points, on depile
		while (Orientation(nextToTop(stack), stack.top(), points[i]) != 2)
			stack.pop();
		stack.push(points[i]);
	}

	int sizeStack = stack.size();

	// Pour fermer l'enveloppe, penser � push le point r�f�rent
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
			maths::Point *p = polygon.getPoints()->at(j);

			std::cout << "x=" << p->x << "   y=" << p->y << std::endl;
			if (mX > p->x - nbX && mX<p->x + nbX && mY>p->y - nbY && mY < p->y + nbY)
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
			
			unsigned int size = polygon.getPoints()->size();

			Point *points= (Point*) malloc(size*sizeof(Point));
			for (int i = 0; i < size; i++)
			{
				points[i] = *(polygon.getPoint(i));
			}

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, size);
			glDisableVertexAttribArray(position_location);

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_LINE_LOOP, 0, size);
			glDisableVertexAttribArray(position_location);
		}

	}
	else if (state == ENTER_POINTS_POLYGON)
	{

		unsigned int size = polygon.getPoints()->size();

		for (int i = 0; i < size; i++)
		{
			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, polygon.getPoint(i));
			glEnableVertexAttribArray(position_location);

			glPointSize(10);

			glDrawArrays(GL_POINTS, 0, 1);
			glDisableVertexAttribArray(position_location);
		}
	}
	else if(state == RUN_JARVIS_MARCH)
	{
		//if(polygon.getPoints()->empty())
		//{
		//	std::cerr << "You forgot to add some points for the Jarvis march algorithm !" << std::endl;
		//}
		//else
		//{
		//	// redondance ici a cause des else if
		//	const maths::Point *points = this->polygon.getPoints()->data();

		//	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
		//	glEnableVertexAttribArray(position_location);

		//	glPointSize(5);

		//	glDrawArrays(GL_POINTS, 0, this->polygon.getPoints()->size());
		//	glDisableVertexAttribArray(position_location);

		//	this->jarvisPoints->clear();
		//	RunJarvis(*(this->polygon.getPoints()), *(this->jarvisPoints));
		//	//ConvexHull(*(this->dataPointsJarvis), (*this->jarvisPoints));
		//	//DrawJarvisPolygon(*(this->dataPointsJarvis), (*this->jarvisPoints));

		//	if(!this->jarvisPoints->empty())
		//	{
		//		glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, this->jarvisPoints->data());
		//		glEnableVertexAttribArray(position_location);

		//		glPointSize(5);

		//		glDrawArrays(GL_LINE_STRIP, 0, this->jarvisPoints->size());
		//		glDisableVertexAttribArray(position_location);
		//	}
		//}

		if (polygon.getPoints()->size()<2)
		{
			std::cerr << "You forgot to add some points for the Delaunay algorithm !" << std::endl;
		}
		else
		{
			// redondance ici a cause des else if
			const maths::Point *points = *(this->polygon.getPoints()->data());

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, this->polygon.getPoints()->size());
			glDisableVertexAttribArray(position_location);

			triangularisation(false);

			


			

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
			for (int i = 0; i < triangulationLines->size(); i++)
			{
				glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, triangulationLines->at(i));
				glEnableVertexAttribArray(position_location);

				glPointSize(5);

				glDrawArrays(GL_LINE, 0, 2);
				glDisableVertexAttribArray(position_location);
			}
		}
	}
	else if(state == RUN_GRAHAM_SCAN)
	{
		const maths::Point *points = *(this->polygon.getPoints()->data());

		glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
		glEnableVertexAttribArray(position_location);

		glPointSize(5);

		glDrawArrays(GL_POINTS, 0, this->polygon.getPoints()->size());
		glDisableVertexAttribArray(position_location);

		if(this->polygon.getPoints()->size() > 3)
		{
			//this->RunGrahamScan((this->polygon.getPoints()), *(this->grahamScanPoints));

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

void Scene::addPoint(maths::Point* p)
{
	switch (state)
	{
	case ENTER_POINTS_POLYGON:
			//std::cout << "point added x=" << p.x << " y=" << p.y << std::endl;
			p->y = height - p->y;
			p->x -= width / 2;
			p->y -= height / 2;
			p->x /= width / 2;
			p->y /= height / 2;
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

	// On calcul la moyenne des coordonn�es des sommets du polygon
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

void Scene::triangularisation(bool makeDelaunay)
{
	for (int i = 0; i < polygon.getPoints()->size(); i++)
	{
		Point *p1 = polygon.getPoint(i);
		int j;
		for (j = 0; j < triangulationPoints->size(); j++)
		{
			Point *p2 = triangulationPoints->at(j);
			if (p2->x == p1->x)
			{
				if (p2->y >= p1->x)
					break;
			}
			if (p2->x > p1->x)
			{
				break;
			}
		}
		triangulationPoints->insert(triangulationPoints->begin() + j, p1);
	}

	int size = triangulationPoints->size();

	triangulationLines->clear();
	triangles->clear();
	if (size < 2)
		return;
	Line *previousLine = createLine(triangulationPoints->at(0), triangulationPoints->at(1));
	triangulationLines->push_back(previousLine);
	int i = 2;
	while (previousLine->isCol(triangulationPoints->at(i)) && i<size)
	{
		previousLine = createLine(previousLine->p2, triangulationPoints->at(i));
		triangulationLines->push_back(previousLine);
		i++;
	}
	if (i == size)
		return;
	Point *p = triangulationPoints->at(i);
	for (int i = 0; i < triangulationLines->size(); i++)
	{
		Line *l1  = triangulationLines->at(i);
		Line *l2 = createLine(l1->p1, p);
		Line *l3 = createLine(l1->p2, p);
		Triangle *t = createTriangle(l1, l2, l3);

		
		
		triangles->push_back(t);
	}

	/*for (; i < triangulationPoints->size(); i++)
	{
		Point *p = triangulationPoints->at(i);
		for (int j = 0; j < triangulationLines->size(); j++)
		{

		}
	}*/

}

void updateLines(Triangle* t)
{
	for (int i = 0; i < 3; i++)
	{
		Line *l, *l2;
		if (i == 0)
		{
			l = t->l1;
			l2 = t->l2;
		}
		else if (i == 1)
		{
			l = t->l2;
			l2 = t->l1;
		}
		else
		{
			l = t->l3;
			l2 = t->l1;
		}
		Point *p;
		if (!l->p1->equals2D(l2->p1) && l->p2->equals2D(l2->p1))
			p = l2->p1;
		else
			p = l2->p2;
		float nb = sign();


	}
}

Line* Scene::createLine(Point* p1, Point* p2)
{
	Line* l = new Line();
	l->p1 = p1;
	l->p2 = p2;
	lines->push_back(l);
	return l;
}


Point* Scene::createPoint(float x, float y)
{
	Point* p = new Point();
	p->x = x;
	p->y = y;
	return p;
}

Triangle* Scene::createTriangle(Line* l1, Line* l2, Line* l3)
{
	Triangle* t = new Triangle();
	t->l1 = l1;
	t->l2 = l2;
	t->l3 = l3;
	return t;
}


float sign(Line* l, Point* p3)
{
	Point *p1, *p2;
	if (l->p1->x < l->p2->x)
	{
		p1 = l->p1;
	}
	else if (l->p1->x > l->p2->x)
	{
		p2 = l->p2;
	}
	else
	{
		if (l->p1->y < l->p2->y)
		{
			p1 = l->p1;
		}
		else
		{
			p2 = l->p2;
		}
	}
	return (p1->x - p3->x) * (p2->y - p3->y) - (p2->x - p3->x) * (p1->y - p3->y);
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


	lines = new std::vector<maths::Line*>(); 
	triangles = new std::vector<maths::Triangle*>();
	triangulationPoints = new std::vector<maths::Point*>();
	triangulationLines = new std::vector<maths::Line*>();

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
