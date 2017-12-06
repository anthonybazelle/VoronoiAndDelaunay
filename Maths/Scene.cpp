#include "Scene.h"

using namespace maths;

Scene* Scene::currentInstance = nullptr;


void Scene::drawCallBack()
{
	Scene::currentInstance->mainLoop();
}

void Scene::flush()
{
	if (state != ENTER_POINTS_POLYGON)
	{
		while (!polygons->empty())
		{
			polygons->pop_back();
		}
		glutPostRedisplay();
	}
}

void Scene::moveSelectedPoint(float x, float y)
{
	Point p;
	p.x = x;
	p.y = y;
	polygons->at(polygonSelected).setPoint(p, pointSelected);
	glutPostRedisplay();
}


void Scene::selectPolygon(float x, float y)
{
	if (state == DRAW)
	{
		maths::Point p;
		p.x = x;
		p.y = y;
		for (int i = 0; i < polygons->size(); i++)
		{
			maths::Polygon pol = polygons->at(i);

			if (isPointInPol(pol, p))
			{
				polygonSelected = i;
				glutPostRedisplay();
				return;
			}
		}
	}
	polygonSelected = -1;
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

	// ATTENDS JE SUIS AU TEL AVEC UN COLLEGUE
	mainMenu = glutCreateMenu(Scene::menuCallBack);

	glutAddMenuEntry("Exit", 0);
	glutAddMenuEntry("Add curve	                   A", 1);
	glutAddMenuEntry("End edition                  E", 2);
	glutAddMenuEntry("Activate translation         W", 3);
	glutAddMenuEntry("Activate rotation            X", 4);
	glutAddMenuEntry("Activate scale               C", 5);
	glutAddMenuEntry("Unactivate all               V", 6);
	glutAddMenuEntry("Link other curve             L", 7);
	glutAddMenuEntry("Draw simple extrude          U", 8);
	glutAddMenuEntry("Draw revolution              R", 9);
	glutAddMenuEntry("Enter points for Jarvis      P", 10);
	glutAddMenuEntry("Run Jarvis March             J", 11);
	/*glutAddMenuEntry("Cut               C", 3);
	glutAddMenuEntry("Fill polygon(s)   F", 4);
	glutAddMenuEntry("Set window        Q", 5);
	glutAddMenuEntry("Select polygon(s) W", 6);
	glutAddMenuEntry("Hide/Show window  P", 7);*/
	/*
	if (stackPolygonClicked->size() != 0)
	{
	glutAddMenuEntry("Fill           F", 4);
	}
	*/
	/*if (isInPolygon)
	{
	glutAddMenuEntry("Coloring polygon", 5);
	}*/

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



void Scene::applyTransformation(char key)
{
	if (polygonSelected != -1 && pointSelected == -1)
	{
		if (activeTransformation == ROTATION)
		{
			int coef = 1;
			if (key == 'd')
				coef = -coef;
			rotate_point(&polygons->at(polygonSelected), coef*3.1416/180);
			glutPostRedisplay();

		}
		else if (activeTransformation == SCALE)
		{
			float coef = 1.1;
			if (key == 's')
				coef = 0.9;

			scalePoint(&polygons->at(polygonSelected), coef);
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

			translatePoint(&polygons->at(polygonSelected), x/width* coef, y/height* coef);
			glutPostRedisplay();
		}
	}
}

void Scene::DrawJarvisPolygon(std::vector<maths::Point>& lp, std::vector<maths::Point>& env)
{
	// recherche du point le plus bas,
	// puis à chaque étape recherche du point qui forme le plus petit
	// angle, avec le segment des 2 points précédents de l'enveloppe
	env.clear();
	// recherche du point le plus bas à droite
	int iBas = 0;
	maths::Point pBas= (maths::Point)lp.at(0);
	for( int i = 1; i < lp.size(); ++i) {
		maths::Point p = ((maths::Point)lp.at(i));
		if(p.y > pBas.y || (p.y == pBas.y && p.x > pBas.x) ) {
			pBas = p;
			iBas = i;
		}
	}
	lp.push_back(pBas);

	double angle = 0.0;
	double anglePrec;
	int m, iMin = iBas;
	for(  m = 0; m < lp.size()-1; ++m){
		this->ChangePosition(lp, iMin, m);
		// recherche du point suivant
		iMin = lp.size()-1;  anglePrec = angle; angle = 2*3.14159265358979323846264338328;
		for( int i = m+1; i < lp.size(); ++i){
			double a = lp.at(m).Angle(lp.at(i)) ;
			if(a > anglePrec && a < angle){
				iMin = i; angle = a;
			}else if( a==angle ){ // points alignés : choisir le plus éloigné
				if(lp.at(iMin).Distance(lp.at(m)) < lp.at(i).Distance(lp.at(m))) 
					iMin = i; 
			}
		}
		if(iMin == lp.size()-1 )break;// on est retombé sur le point de départ
	}
	for( int i = 0; i <= m; ++i) env.push_back(lp.at(i));
	lp.erase(lp.begin() + (lp.size()-1));
}

void Scene::ChangePosition(std::vector<maths::Point>& lp, int indice1, int indice2)
{
	std::iter_swap(lp.begin() + indice1, lp.begin() + indice2);
}

void Scene::changeBezierRecursion(int nb)
{
	if (polygonSelected != -1)
	{
		polygons->at(polygonSelected).changeBezierRecursion(nb);
	}
	else
	{
		for (int i = 0; i < polygons->size(); i++)
		{
			polygons->at(i).changeBezierRecursion(nb);
		}
	}
}

void Scene::changeActiveTransformation(Transformation trans)
{
	activeTransformation = trans;
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
	case 7:
		input->checkKeyboardInputs('l', 0, 0);
		break;
	case 8:
		input->checkKeyboardInputs('r', 0, 0);
		break;
	case 9:
		input->checkKeyboardInputs('u', 0, 0);
		break;
	case 10:
		input->checkKeyboardInputs('p', 0, 0);
		break;
	case 11:
		input->checkKeyboardInputs('j', 0, 0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}


void Scene::linkOtherCurve()
{
	if (polygonSelected != -1 && pointSelected == -1)
	{
		maths::Polygon p1 = polygons->at(polygonSelected);
		if (p1.getOutPolygon() == NULL)
		{
			changeState(ENTER_POINTS_POLYGON);
			maths::Polygon p2 = polygons->at(polygons->size() - 1);

			polygons->at(polygonSelected).setOutPolygon(&polygons->at(polygons->size() - 1));
			polygons->at(polygons->size() - 1).setInPolygon(&polygons->at(polygonSelected));

			p2.addPoint(p1.getPoints()->at((p1.getPoints()->size() - 1)));
		}

	}
}

bool Scene::isPointSelected(float mX, float mY)
{
	if (state == DRAW)
	{
		float nb = 10;
		float nbX = nb /width;
		float nbY = nb / height;

		std::cout << "mx = " << mX <<"  mY=" << mY << std::endl;
		for (int i = 0; i < polygons->size(); i++)
		{
			for (int j = 0; j < polygons->at(i).getPoints()->size(); j++)
			{
				maths::Point p = polygons->at(i).getPoints()->at(j);

				std::cout << "x=" << p.x <<"   y=" << p.y << std::endl;
				if (mX > p.x - nbX && mX<p.x + nbX && mY>p.y - nbY && mY < p.y + nbY)
				{
					if (polygonSelected == i)
					{
						pointSelected = j;
					}
					else
					{
						pointSelected = -1;
						polygonSelected = i;
					}
					return true;
				}
			}
		}
	}
	pointSelected = -1;
	polygonSelected = -1;

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
		if (!polygons->empty())
		{
			for (int i = 0; i < polygons->size(); i++)
			{
				polygons->at(i).recalculateBezierPoints(width,height);

				const maths::Point *bezierPoints = polygons->at(i).getBezierPoints()->data();
				unsigned int bezierSize = polygons->at(i).getBezierPoints()->size();


				if (i == polygonSelected)
				{
					glUniform4f(colorID, 1.0,0.0,0.0,1.0);
				}
				else
				{
					glUniform4f(colorID, color[0], color[1], color[2], color[3]);
				}

				if(this->intermediateState == NONE)
				{
					glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, bezierPoints);
					glEnableVertexAttribArray(position_location);

					glPointSize(5);

					glDrawArrays(GL_LINE_STRIP, 0, bezierSize);
					glDisableVertexAttribArray(position_location);

					if (pointSelected != -1 && i == polygonSelected)
					{
						const maths::Point *points = polygons->at(i).getPoints()->data();
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
						size = polygons->at(i).getPoints()->size()- pointSelected-1;

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
						const maths::Point *points = polygons->at(i).getPoints()->data();
						unsigned int size = polygons->at(i).getPoints()->size();

						glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
						glEnableVertexAttribArray(position_location);

						glPointSize(5);

						glDrawArrays(GL_POINTS, 0, size);
						glDisableVertexAttribArray(position_location);
					}
				}
				// On vérifie que l'on a bien une courbe au moins de dessinée en plus de l'etat à vérifier
				else if(this->intermediateState == EXTRUDE && polygons->size() > 0)
				{
					const maths::Point *bezierPointsExtrude = polygons->at(i).getBezierPoints()->data();
					std::vector<maths::Point>* offsetBezierPoints = new std::vector<maths::Point>();

					for(int i = 0; i < polygons->at(i).getBezierPoints()->size(); i++)
					{
						maths::Point offsetPoint = polygons->at(i).getBezierPoints()->at(i);
						// A paramétrer
						offsetPoint.z += 0.2f;
						offsetBezierPoints->push_back(offsetPoint);
					}

					// Traitement OpenGL
				}
				else if(this->intermediateState == REVOLUTION && polygons->size() > 0)
				{

				}
			}
		}

	}
	else if (state == ENTER_POINTS_POLYGON)
	{
		for (int i = 0; i < polygons->size() - 1; i++)
		{
			//polygons->at(i).recalculateBezierPoints();

			const maths::Point *bezierPoints = polygons->at(i).getBezierPoints()->data();
			unsigned int bezierSize = polygons->at(i).getBezierPoints()->size();

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, bezierPoints);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_LINE_STRIP, 0, bezierSize);
			glDisableVertexAttribArray(position_location);

			const maths::Point *points = polygons->at(i).getPoints()->data();
			unsigned int size = polygons->at(i).getPoints()->size();

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, size);
			glDisableVertexAttribArray(position_location);
		}

		const maths::Point *points = polygons->back().getPoints()->data();
		unsigned int size = polygons->back().getPoints()->size();

		for (int i = 0; i < size; i++)
		{
			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, &points[i]);
			glEnableVertexAttribArray(position_location);

			glPointSize(10);

			glDrawArrays(GL_POINTS, 0, 1);
			glDisableVertexAttribArray(position_location);
		}
	}
	else if(state == ENTER_POINTS_JARVIS)
	{
		if(!dataPointsJarvis->empty())
		{
			const maths::Point *points = this->dataPointsJarvis->data();

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, this->dataPointsJarvis->size());
			glDisableVertexAttribArray(position_location);
		}
	}
	else if(state == RUN_JARVIS_MARCH)
	{
		if(dataPointsJarvis->empty())
		{
			std::cerr << "You forgot to add some points for the Jarvis march algorithm !" << std::endl;
		}
		else
		{
			// redondance ici a cause des else if
			const maths::Point *points = this->dataPointsJarvis->data();

			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, points);
			glEnableVertexAttribArray(position_location);

			glPointSize(5);

			glDrawArrays(GL_POINTS, 0, this->dataPointsJarvis->size());
			glDisableVertexAttribArray(position_location);



			DrawJarvisPolygon(*(this->dataPointsJarvis), (*this->jarvisPoints));

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

	glUseProgram(0);

	glutSwapBuffers();
}

void Scene::changeState(State s)
{
	if (state == s)
		return;
	state = s;
	switch (state)
	{
	case ENTER_POINTS_POLYGON:
		polygons->push_back(*(new maths::Polygon()));
		break;
	case ENTER_WINDOW:
		window->getPoints()->clear();
		break;
	case DRAW:
		break;
	case FILL:
		allIntersection->clear();
		for (int i = 0; i < polygons->size(); i++)
		{
			allIntersection->push_back(LCARemplissage(polygons->at(i)));
		}
		break;
	default:
		break;
	}
}

void Scene::changeIntermediateState(IntermediateState s)
{
	if(this->intermediateState == s)
		return;

	this->intermediateState = s;
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
		if (!polygons->empty())
		{
			//std::cout << "point added x=" << p.x << " y=" << p.y << std::endl;
			p.y = height - p.y;
			p.x -= width / 2;
			p.y -= height / 2;
			p.x /= width / 2;
			p.y /= height / 2;
			//std::cout << "point normalized x=" << p.x << " y=" << p.y << std::endl;

			polygons->back().addPoint(p);
		}
		break;
	case ENTER_WINDOW:
		//std::cout << "point added x=" << p.x << " y=" << p.y << std::endl;
		p.x -= width / 2;
		p.x /= width / 2;
		p.y = height - p.y;
		p.y -= height / 2;
		p.y /= height / 2;
		p.x = Math::round(p.x);
		p.y = Math::round(p.y);
		//std::cout << "point normalized x=" << p.x << " y=" << p.y << std::endl;
		window->addPoint(p);
		break;
	case ENTER_POINTS_JARVIS:
		p.y = height - p.y;
		p.x -= width / 2;
		p.y -= height / 2;
		p.x /= width / 2;
		p.y /= height / 2;
		this->dataPointsJarvis->push_back(p);
		break;
	case DRAW:
		break;
	case FILL:
		break;
	case COLOR:
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

// Paramètre : polygon que l'on veut colorier
// Retour : tableau de points (traités comme des segments) qui doivent être colorié dans la mainLoop
// But :  Détecter quelles zones sont à colorier
std::vector<maths::Point>* Scene::LCARemplissage(maths::Polygon polygon)
{
	std::vector<maths::Point>* allPointToDraw = new std::vector<maths::Point>();

	for (int y = 1; y < height; y++)
	{
		int bitParite = 0;

		// On créé notre ligne de balayage
		std::vector<maths::Point>* ligneBallayage = new std::vector<maths::Point>();

		maths::Point posXLigneBalayage;
		posXLigneBalayage.x = 0;
		posXLigneBalayage.y = y;

maths:Point posYLigneBalayage;
		posYLigneBalayage.x = width;
		posYLigneBalayage.y = y;

		ligneBallayage->push_back(posXLigneBalayage);
		ligneBallayage->push_back(posYLigneBalayage);

		// On récupère les points du polygon
		std::vector<maths::Point>* pointsFromPolygon = polygon.getPoints();
		int nbPoint = polygon.getPoints()->size();

		// On test les intersections entre la ligne de balayage et tous les côtés du polygon
		std::vector<maths::Point>* pointsIntersection = new std::vector<maths::Point>();
		for (int i = 0; i < nbPoint; i++)
		{
			maths::Point pointIntersection;

			// Attention ici on converti les points du repère pixel de l'écran au repère OpenGL
			maths::Point* pointXLigneBalayage = ConvertPointPixelToOpenGLUnit(ligneBallayage->at(0));
			maths::Point* pointYLigneBalayage = ConvertPointPixelToOpenGLUnit(ligneBallayage->at(1));

			// Petit cas particulier pour le dernier point que l'on associe au premier point pour tester le côté qui ferme le polygon
			// TODO : Trier intersection avec les segments ayant la norme la plus petite par rapport au bord gauche de la fenêtre
			if (i == nbPoint - 1)
			{
				pointIntersection = CVecteur::Intersection(*pointXLigneBalayage, *pointYLigneBalayage, pointsFromPolygon->at(i), pointsFromPolygon->at(0));
			}
			else
			{
				pointIntersection = CVecteur::Intersection(*pointXLigneBalayage, *pointYLigneBalayage, pointsFromPolygon->at(i), pointsFromPolygon->at(i + 1));
			}

			if (pointIntersection.x != -1 && pointIntersection.y != -1)
			{
				// TODO: Faire le tri ici
				//Fait ! :)
				int i = 0;
				while (i < pointsIntersection->size())
				{
					if (pointsIntersection->at(i).x > pointIntersection.x)
						break;
					i++;
				}
				if (i == pointsIntersection->size())
					pointsIntersection->push_back(pointIntersection);
				else
					pointsIntersection->insert(pointsIntersection->begin() + i, pointIntersection);
			}
		}

		if (pointsIntersection->size() != 0)
		{
			// On dessine entre chaque intersection avec la bonne couleur
			for (int i = 0; i < pointsIntersection->size(); i++)
			{
				// On teste le cas particulier du passage de la ligne de balayage sur un sommet du polygon
				maths::Point* peakDetect = isVertexFromPolygon(polygon, pointsIntersection->at(i));
				if (peakDetect != nullptr)
				{
					// On ajoute 2 fois car on traitra les points contenus dans le tableau 2 à 2 (segement)
					allPointToDraw->push_back(*peakDetect);
					allPointToDraw->push_back(*peakDetect);
				}
				else
				{
					allPointToDraw->push_back(pointsIntersection->at(i));
				}
			}
		}
	}

	return allPointToDraw;
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
	for (int i = 0; i < polygons->size(); i++)
	{
		bool dog = isPointInPol(polygons->at(i), p);

		if (dog == true)
		{
			stackPolygonClicked->push_back(polygons->at(i));
		}
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

Scene::Scene(int w, int h)
{
	window = new maths::Polygon();
	state = DRAW;
	this->intermediateState = NOCHOSEN;
	height = h;
	width = w;
	value = 0;
	drawWindow = true;
	isInPolygon = true;
	Scene::currentInstance = this;
	input = new Input(this);
	polygons = new std::vector<maths::Polygon>();
	allIntersection = new std::vector<std::vector<maths::Point>*>();
	stackPolygonClicked = new std::vector<maths::Polygon>();

	this->dataPointsJarvis = new std::vector<maths::Point>();
	this->jarvisPoints = new std::vector<maths::Point>();

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
