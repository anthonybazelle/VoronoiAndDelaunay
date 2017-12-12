#include "glew.h"
#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib") 
#endif
#include <iostream>

#include "freeglut.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm> 
#include <functional> 
#include <set> 
#define PI 3.14159265

struct Point;
struct Apex;
struct Edge;
struct Bounded_Edge;
struct Triangle;
struct Circle;

typedef struct Point
{
	float x;
	float y;
} Point;

bool operator==(const Point& p1, const Point& p2)
{
	if (p1.x == p2.x && p1.y == p2.y)
	{
		return true;
	}

	return false;
}

typedef struct Apex
{
	Point* p;
	Edge* a;

	Apex() : p(nullptr), a(nullptr) {};

	Apex(Point* point) : p(point), a(nullptr) {};
} Apex;

bool operator==(const Apex& s1, const Apex& s2)
{
	if (*s1.p == *s2.p)
	{
		return true;
	}

	return false;
}

typedef struct Edge
{
	Apex* s1;
	Apex* s2;
	Triangle* td;
	Triangle* tg;

	Edge() : s1(nullptr), s2(nullptr), td(nullptr), tg(nullptr) {};

	Edge(Apex* s1, Apex* s2) : s1(s1), s2(s2), td(nullptr), tg(nullptr) {};
} Edge;

bool operator==(const Edge& a1, const Edge& a2)
{
	if (a1.s1 == a2.s1 && a1.s2 == a2.s2)
	{
		return true;
	}

	return false;
}

typedef struct Bounded_Edge
{
	Point p1;
	Point p2;

	Bounded_Edge(Point p1, Point p2) : p1(p1), p2(p2) {};
} Bounded_Edge;

typedef struct Triangle
{
	Edge* a1;
	Edge* a2;
	Edge* a3;

	Triangle() : a1(nullptr), a2(nullptr), a3(nullptr) {};

	Triangle(Edge* e1, Edge* e2, Edge* e3) : a1(e1), a2(e2), a3(e3) {};
} Triangle;

bool operator==(const Triangle& t1, const Triangle& t2)
{
	if (t1.a1 == t2.a1 && t1.a2 == t2.a2 && t1.a3 == t2.a3)
	{
		return true;
	}

	return false;
}

typedef struct Circle
{
	Point center;
	float radius;
} Circle;

struct points_comparator
{
	inline bool operator() (const Point& p1, const Point& p2)
	{
		if (p1.x < p2.x) return true;
		if (p1.x > p2.x) return false;
		if (p1.y < p2.y) return true;
		return false;
	}
};

//Triangulation elements
std::vector<Point> points;
std::vector<Apex> apexes;
std::vector<Edge> edges;
std::vector<Triangle> triangles;
std::map<Apex*, std::vector<Bounded_Edge>> voronoi_regions;

//Triangulation parameters
int maxPoints = 50;
float colinearity_tolerance = 0.0001;
bool voronoi;

//Glut parameters
int menuPrincipal = 0;
int colorSubMenu;
int voronoiColorSubMenu;
std::tuple<float, float, float> triangulationColor;
std::tuple<float, float, float> voronoiColor;

//Glut functions
void dessin();
void mouse(int button, int state, int x, int y);
void showMenu();
void mainMenuCallback(int);
void colorMenuCallBack(int);
void voronoiColorMenuCallBack(int);

//Triangulation functions
void triangulate();
void delaunayTriangulation();
void voronoiDiagram();
void delaunayAddPoint();
void delaunayDeletePoint(Point*);

//Utility functions
bool isTriangleDirect(Triangle);
bool isTriangleLeft(Triangle, Edge*);
float determinant(Apex*, Apex*, Apex*);
bool delaunayCriteria(Edge*);
std::tuple<Apex*, Apex*, Apex*> apexesOfTriangle(Triangle*);
Circle circonscriptCircle(Apex*, Apex*, Apex*);
bool pointIsInTriangle(Triangle*, Apex*);
std::vector<Edge*> findSeenEdges(Apex*);
std::vector<Edge*> findAdjacentEdges(Point*);
float orientedAngle(Edge*, Edge*, Point*);
std::vector<Edge*> findOrientedAdjacentEdges(Point*);
std::vector<Triangle*> findOrientedAdjacentTriangles(std::vector<Edge*>);
std::vector<Edge*> findNonAdjacentEdges(std::vector<Edge*>, std::vector<Triangle*>);
void addTriangleToItsEdges(Triangle*);
bool edgeExists(Apex*, Apex*);

int main(int argc, char **argv)
{
	/*r.push_back(0.0f);
	g.push_back(0.0f);
	b.push_back(0.0f);*/
	voronoi = false;
	points.reserve(maxPoints);
	apexes.reserve(maxPoints);
	edges.reserve(maxPoints * (maxPoints - 1) / 2);
	triangles.reserve(maxPoints * maxPoints);


	// Initialisation de glut et creation de la fenetre
	// ------------------------------------------------
	glutInit(&argc, argv);											// Initialisation
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);		// Mode d'affichage RGB, et test prafondeur

	glutInitWindowSize(500, 500);									// Dimension fenêtre
	glutInitWindowPosition(100, 100);								// Position coin haut gauche
	glutCreateWindow("Triangulation");								// Nom
	gluOrtho2D(-250.0, 250.0, -250.0, 250.0);						// Repère 2D délimitant les abscisses et les ordonnées


																	// Initialisation d'OpenGL
																	// -----------------------
	glClearColor(1.0, 1.0, 1.0, 0.50);
	glColor3f(1.0, 1.0, 1.0);			     	 // couleur: blanc
	glPointSize(2.0);							 // taille d'un point: 2px
	glLineWidth(1);

												 /* Enregistrement des fonctions de rappel
												 => initialisation des fonctions callback appelées par glut */
	

	showMenu();
	glutMouseFunc(mouse);
	//glutMotionFunc(mouse_activeFunc);
	glutDisplayFunc(dessin);


	/* rq: le callback de fonction (fonction de rappel) est une fonction qui est passée en argument à une
	autre fonction. Ici, le main fait usage des deux fonctions de rappel (qui fonctionnent en même temps)
	alors qu'il ne les connaît pas par avance.*/



	/* Entrée dans la boucle principale de glut, traitement des évènements */
	
	glutMainLoop();								  // lancement de la boucle de réception des évènements
	return 0;
}

void dessin()
{
	float r, g, b;

	glClearColor(1.0, 1.0, 1.0, 0.50);
	glClear(GL_COLOR_BUFFER_BIT);
	std::tie(r, g, b) = triangulationColor;

	glBegin(GL_POINTS);
	glColor3f(r, g, b);
	for (int i = 0; i < apexes.size(); i++)
	{
		glVertex2f(apexes.at(i).p->x, apexes.at(i).p->y);
	}

	glEnd();
	glutSwapBuffers();

	glBegin(GL_LINES);
	glColor3f(r, g, b);
	for (int k = 0; k < edges.size(); ++k)
	{
		glVertex2f(edges.at(k).s1->p->x, edges.at(k).s1->p->y);
		glVertex2f(edges.at(k).s2->p->x, edges.at(k).s2->p->y);
	}

	glEnd();
	glutSwapBuffers();

	if (voronoi)
	{
		std::tie(r, g, b) = voronoiColor;
		glColor3f(r, g, b);

		glBegin(GL_LINES);

		for (int i = 0; i < apexes.size(); i++)
		{
			for (int k = 0; k < voronoi_regions.at(&apexes.at(i)).size(); k++)
			{
				glVertex2f(voronoi_regions.at(&apexes.at(i)).at(k).p1.x, 
					voronoi_regions.at(&apexes.at(i)).at(k).p1.y);
				glVertex2f(voronoi_regions.at(&apexes.at(i)).at(k).p2.x,
					voronoi_regions.at(&apexes.at(i)).at(k).p2.y);
			}
		}

		glEnd();
		glutSwapBuffers();
	}
}

void mouse(int button, int state, int x, int y)
{
	Point tmp;
	bool deletePoint = false;
	Point* p = nullptr;

	if (points.size() == maxPoints)
	{
		return;
	}
	
	// Si on appuie sur le bouton de gauche	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		tmp.x = x - 250.0f;
		tmp.y = -y + 250.0f;

		for (int i = 0; i < points.size(); ++i)
		{
			if (tmp.x == points.at(i).x && tmp.y == points.at(i).y)
			{
				p = &points.at(i);
				deletePoint = true;
				break;
			}
		}

		if (deletePoint)
		{
			points.erase(std::find(points.begin(), points.end(), *p));
		}
		else
		{
			points.push_back(tmp);
		}
		//voronoi = false;
		triangulate();
		delaunayTriangulation();

		if (voronoi)
		{
			voronoiDiagram();
		}

	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
	{
		tmp.x = x - 250.0f;
		tmp.y = -y + 250.0f;
		//voronoi = false;

		for (int i = 0; i < points.size(); ++i)
		{
			if (tmp.x == points.at(i).x && tmp.y == points.at(i).y)
			{
				p = &points.at(i);
				deletePoint = true;
				break;
			}
		}

		if (deletePoint)
		{
			delaunayDeletePoint(p);
		}
		else
		{
			points.push_back(tmp);
			delaunayAddPoint();
		}

		if (voronoi)
		{
			voronoiDiagram();
		}
	}
}

void showMenu()
{
	glutDestroyMenu(menuPrincipal);

	colorSubMenu = glutCreateMenu(colorMenuCallBack);
	glutAddMenuEntry("Blanc", 11);
	glutAddMenuEntry("Jaune", 12);
	glutAddMenuEntry("Vert", 13);
	glutAddMenuEntry("Noir", 14);
	glutAddMenuEntry("Bleu", 15);
	glutAddMenuEntry("Orange", 16);
	glutAddMenuEntry("Violet", 17);

	voronoiColorSubMenu = glutCreateMenu(voronoiColorMenuCallBack);
	glutAddMenuEntry("Blanc", 21);
	glutAddMenuEntry("Jaune", 22);
	glutAddMenuEntry("Vert", 23);
	glutAddMenuEntry("Noir", 24);
	glutAddMenuEntry("Bleu", 25);
	glutAddMenuEntry("Orange", 26);
	glutAddMenuEntry("Violet", 27);

	menuPrincipal = glutCreateMenu(mainMenuCallback);

	glutAddSubMenu("Couleur", colorSubMenu);
	glutAddSubMenu("Voronoi", voronoiColorSubMenu);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void mainMenuCallback(int menuItem)
{	
	showMenu();
}

void colorMenuCallBack(int a)
{
	switch (a)
	{
	case 12:
		triangulationColor = std::make_tuple(1.0f, 1.0f, 0.0f);
		break;
	case 13:
		triangulationColor = std::make_tuple(0.0f, 1.0f, 0.0f);
		break;
	case 14:
		triangulationColor = std::make_tuple(0.0f, 0.0f, 0.0f);
		break;
	case 15:
		triangulationColor = std::make_tuple(0.0f, 0.0f, 1.0f);
		break;
	case 16:
		triangulationColor = std::make_tuple(1.0f, 0.5f, 0.0f);
		break;
	case 17:
		triangulationColor = std::make_tuple(1.0f, 0.0f, 1.0f);
		break;
	default:
		triangulationColor = std::make_tuple(1.0f, 1.0f, 1.0f);
		break;
	}
}

void voronoiColorMenuCallBack(int a)
{
	voronoi = true;

	switch (a)
	{
	case 22:
		voronoiColor = std::make_tuple(1.0f, 1.0f, 0.0f);
		break;
	case 23:
		voronoiColor = std::make_tuple(0.0f, 1.0f, 0.0f);
		break;
	case 24:
		voronoiColor = std::make_tuple(0.0f, 0.0f, 0.0f);
		break;
	case 25:
		voronoiColor = std::make_tuple(0.0f, 0.0f, 1.0f);
		break;
	case 26:
		voronoiColor = std::make_tuple(1.0f, 0.5f, 0.0f);
		break;
	case 27:
		voronoiColor = std::make_tuple(1.0f, 0.0f, 1.0f);
		break;
	default:
		voronoiColor = std::make_tuple(1.0f, 1.0f, 1.0f);
		break;
	}
}

void triangulate()
{
	//Reseting previous triangulation
	apexes.clear();
	edges.clear();
	triangles.clear();


	//Sorting points by coordinates
	//-----------------------------
	std::sort(points.begin(), points.end(), points_comparator());


	//Initializing 2-triangulation
	//----------------------------
	int i = 0;

	if (points.size() == 0)
	{
		return;
	}

	if (points.size() == 1)
	{
		apexes.push_back(Apex(&points.at(i)));
		return;
	}

	apexes.push_back(Apex(&points.at(i)));
	apexes.push_back(Apex(&points.at(i + 1)));
	edges.push_back(Edge(&apexes.at(i), &apexes.at(i + 1)));
	apexes.at(i).a = &edges.at(i);
	apexes.at(i + 1).a = &edges.at(i);
	i += 2;

	//if point colinear, add it
	while (i < points.size()
		&&	abs(((points.at(i - 2).y - points.at(i - 1).y) * (points.at(i - 2).x - points.at(i).x))
			- ((points.at(i - 2).y - points.at(i).y) * (points.at(i - 2).x - points.at(i - 1).x)))
		<= colinearity_tolerance)
	{
		apexes.push_back(Apex(&points.at(i)));
		edges.push_back(Edge(&apexes.at(i - 1), &apexes.at(i)));
		apexes.at(i).a = &edges.at(i - 1);
		++i;
	}

	if (i == points.size())
	{
		return;
	}

	int nedges = edges.size();
	//triangulate all colinear points to first non-colinear one
	for (int j = 0; j < nedges; ++j)
	{
		apexes.push_back(Apex(&points.at(i)));

		if (j == 0)
		{
			edges.push_back(Edge(&apexes.at(i), edges.at(j).s1));
			apexes.at(i).a = &edges.at(i - 1);
		}

		edges.push_back(Edge(edges.at(j).s2, &apexes.at(i)));

		Triangle t = Triangle(&edges.at(j), &edges.at(edges.size() - 2), &edges.at(edges.size() - 1));

		if (!isTriangleDirect(t))
		{
			t = Triangle(&edges.at(j), &edges.at(edges.size() - 1), &edges.at(edges.size() - 2));
		}

		triangles.push_back(t);

		addTriangleToItsEdges(&triangles.back());

		if (j > 0)
		{
			edges.at(edges.size() - 3).tg = &triangles.at(j);
		}
	}

	//Incrementing triangulation
	//--------------------------
	float d, dref;
	Triangle* tref;
	Apex a;
	Edge e1;
	Edge e2;
	Triangle tr;
	std::vector<Apex*> tr_apexes;
	tr_apexes.reserve(2);
	std::vector<Edge*> seen_edges;
	seen_edges.reserve(maxPoints - 1);
	std::map<Apex*, Edge*> added_edges;
	std::map<Apex*, Edge*>::iterator it;
	Edge* tr_e1;
	Edge* tr_e2;

	for (int j = i + 1; j < points.size(); ++j)
	{
		//add point to triangulation apexes
		apexes.push_back(Apex(&points.at(j)));

		//find seen edges
		seen_edges = findSeenEdges(&apexes.back());

		//triangulate seen edges
		for (int k = 0; k < seen_edges.size(); ++k)
		{
			e1.s1 = &apexes.at(j);
			e1.s2 = seen_edges.at(k)->s1;
			e2.s1 = &apexes.at(j);
			e2.s2 = seen_edges.at(k)->s2;

			it = added_edges.find(seen_edges.at(k)->s1);
			//add new edge(s)
			if (it == added_edges.end())
			{
				edges.push_back(e1);
				added_edges.insert(std::pair<Apex*, Edge*>(seen_edges.at(k)->s1, &edges.back()));
				tr_e1 = &edges.back();
			}
			else
			{
				tr_e1 = it->second;
			}

			it = added_edges.find(seen_edges.at(k)->s2);
			if (it == added_edges.end())
			{
				edges.push_back(e2);
				added_edges.insert(std::pair<Apex*, Edge*>(seen_edges.at(k)->s2, &edges.back()));
				tr_e2 = &edges.back();
			}
			else
			{
				tr_e2 = it->second;
			}

			//create new triangle
			tr = Triangle(seen_edges.at(k), tr_e1, tr_e2);

			if (!isTriangleDirect(tr))
			{
				tr = Triangle(seen_edges.at(k), tr_e2, tr_e1);
			}

			//add new triangle to triangulation
			triangles.push_back(tr);

			//add triangle to edges
			addTriangleToItsEdges(&triangles.back());
		}

		//clear seen edges for next point
		seen_edges.clear();
		added_edges.clear();
	}
}

void delaunayTriangulation()
{
	std::vector<Edge*> edges_copy;
	edges_copy.reserve(maxPoints * (maxPoints - 1) / 2);

	for (int i = 0; i < edges.size(); ++i)
	{
		edges_copy.push_back(&edges.at(i));
	}

	Edge *a;
	Apex *s1, *s2, *s3, *s4;
	Edge *a1, *a2, *a3, *a4;
	Triangle *t1, *t2;

	while (!edges_copy.empty())
	{
		a = edges_copy.back();
		edges_copy.pop_back();

		if (delaunayCriteria(a))
		{
			continue;
		}

		t1 = a->tg;
		t2 = a->td;
		
		if (t1->a1 == a)
		{
			a1 = t1->a2;
			a4 = t1->a3;
		}
		else if (t1->a2 == a)
		{
			a1 = t1->a3;
			a4 = t1->a1;
		}
		else
		{
			a1 = t1->a1;
			a4 = t1->a2;
		}

		if (t2->a1 == a)
		{
			a2 = t2->a3;
			a3 = t2->a2;
		}
		else if (t2->a2 == a)
		{
			a2 = t2->a1;
			a3 = t2->a3;
		}
		else
		{
			a2 = t2->a2;
			a3 = t2->a1;
		}

		s1 = a->s1;
		s2 = a->s2;

		if (a1->s1 == a->s1 || a1->s1 == a->s2)
		{
			s3 = a1->s2;
		}
		else
		{
			s3 = a1->s1;
		}

		if (a3->s1 == a->s1 || a3->s1 == a->s2)
		{
			s4 = a3->s2;
		}
		else
		{
			s4 = a3->s1;
		}

		a->s1 = s3;
		a->s2 = s4;
		s1->a = a1;
		s2->a = a4;

		t1->a1 = a;
		t1->a2 = a1;
		t1->a3 = a2;

		t2->a1 = a;
		t2->a2 = a3;
		t2->a3 = a4;

		if (!isTriangleDirect(*t1))
		{
			t1->a2 = a2;
			t1->a3 = a1;
		}
		if (!isTriangleDirect(*t2))
		{
			t2->a2 = a4;
			t2->a3 = a3;
		}

		for (Edge edge : edges)
		{
			if (edge.td == t1 || edge.td == t2)
			{
				edge.td = nullptr;
			}
			if (edge.tg == t1 || edge.tg == t2)
			{
				edge.tg = nullptr;
			}
		}

		a->td = nullptr;
		a->tg = nullptr;

		addTriangleToItsEdges(t1);
		addTriangleToItsEdges(t2);

		if (std::find(edges_copy.begin(), edges_copy.end(), a1) == edges_copy.end())
		{
			edges_copy.push_back(a1);
		}
		if (std::find(edges_copy.begin(), edges_copy.end(), a2) == edges_copy.end())
		{
			edges_copy.push_back(a2);
		}
		if (std::find(edges_copy.begin(), edges_copy.end(), a3) == edges_copy.end())
		{
			edges_copy.push_back(a3);
		}
		if (std::find(edges_copy.begin(), edges_copy.end(), a4) == edges_copy.end())
		{
			edges_copy.push_back(a4);
		}
	}
}

void voronoiDiagram()
{
	voronoi_regions.clear();

	Circle c;
	Apex *s1, *s2, *s3;
	Apex *s4, *s5, *s6;
	std::map<Triangle*, Circle> circonscript_circles;
	std::map<Triangle*, Circle>::iterator it0;
	std::map<Triangle*, Circle>::iterator it1;
	std::map<Edge*, Bounded_Edge> bounded_edges;
	std::map<Edge*, Bounded_Edge>::iterator it2;
	Circle c1, c2;
	Point middle;

	for (int i = 0; i < triangles.size(); ++i)
	{
		std::tie(s1, s2, s3) = apexesOfTriangle(&triangles.at(i));
		c = circonscriptCircle(s1, s2, s3);

		circonscript_circles.insert(std::pair<Triangle*, Circle>(&triangles.at(i), c));
	}
	
	for (int i = 0; i < edges.size(); ++i)
	{
		if (edges.at(i).td != nullptr && edges.at(i).tg != nullptr)
		{
			it0 = circonscript_circles.find(edges.at(i).td);
			c1 = it0->second;
			it1 = circonscript_circles.find(edges.at(i).tg);
			c2 = it1->second;

			bounded_edges.insert(std::pair<Edge*, Bounded_Edge>(&edges.at(i), Bounded_Edge(c1.center, c2.center)));
		}
		else
		{
			float dref, d;
			Apex s;

			if (edges.at(i).td == nullptr)
			{
				it1 = circonscript_circles.find(edges.at(i).tg);
				c1 = it1->second;

				middle.x = (edges.at(i).s1->p->x + edges.at(i).s2->p->x) / 2;
				middle.y = (edges.at(i).s1->p->y + edges.at(i).s2->p->y) / 2;

				std::tie(s4, s5, s6) = apexesOfTriangle(edges.at(i).tg);
				if (s4 != edges.at(i).s1 && s4 != edges.at(i).s2)
				{
					dref = (s4->p->x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (s4->p->y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (c1.center.y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s5 != edges.at(i).s1 && s5 != edges.at(i).s2)
				{
					dref = (s5->p->x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (s5->p->y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (c1.center.y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s6 != edges.at(i).s1 && s6 != edges.at(i).s2)
				{
					dref = (s6->p->x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (s6->p->y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (c1.center.y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
			}
			else
			{
				it1 = circonscript_circles.find(edges.at(i).td);
				c1 = it1->second;

				middle.x = (edges.at(i).s1->p->x + edges.at(i).s2->p->x) / 2;
				middle.y = (edges.at(i).s1->p->y + edges.at(i).s2->p->y) / 2;

				std::tie(s4, s5, s6) = apexesOfTriangle(edges.at(i).td);
				if (s4 != edges.at(i).s1 && s4 != edges.at(i).s2)
				{
					dref = (s4->p->x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (s4->p->y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (c1.center.y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s5 != edges.at(i).s1 && s5 != edges.at(i).s2)
				{
					dref = (s5->p->x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (s5->p->y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (c1.center.y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
				else if (s6 != edges.at(i).s1 && s6 != edges.at(i).s2)
				{
					dref = (s6->p->x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (s6->p->y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

					d = (c1.center.x - edges.at(i).s1->p->x) * (edges.at(i).s2->p->y - edges.at(i).s1->p->y)
						- (c1.center.y - edges.at(i).s1->p->y) * (edges.at(i).s2->p->x - edges.at(i).s1->p->x);
					d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

					if (d != dref)
					{
						continue;
					}
				}
			}

			bounded_edges.insert(std::pair<Edge*, Bounded_Edge>(&edges.at(i), Bounded_Edge(c1.center, middle)));
		}
	}
	
	for (int i = 0; i < apexes.size(); ++i)
	{
		std::vector<Bounded_Edge> region;

		for (int j = 0; j < edges.size(); ++j)
		{
			if (edges.at(j).s1 == &apexes.at(i) || edges.at(j).s2 == &apexes.at(i))
			{
				it2 = bounded_edges.find(&edges.at(j));
				if (it2 == bounded_edges.end())
				{
					continue;
				}
				region.push_back(it2->second);
			}
		}

		voronoi_regions.insert(std::pair<Apex*, std::vector<Bounded_Edge>>(&apexes.at(i), region));
		region.clear();
	}
}

void delaunayAddPoint()
{
	if (triangles.empty())
	{
		if (apexes.empty())
		{
			apexes.push_back(Apex(&points.back()));
			return;
		}

		if (apexes.size() == 1)
		{
			apexes.push_back(Apex(&points.back()));
			edges.push_back(Edge(&apexes.at(0), &apexes.back()));
			apexes.at(0).a = &edges.back();
			apexes.at(1).a = &edges.back();
			return;
		}

		if(abs(((apexes.at(0).p->y - apexes.at(1).p->y) * (apexes.at(0).p->x - points.back().x))
			- ((apexes.at(0).p->y - points.back().y) * (apexes.at(0).p->x - apexes.at(1).p->x)))
			<= colinearity_tolerance)
		{
			if (points.back().x < apexes.at(0).p->x 
				|| (points.back().x == apexes.at(0).p->x && points.back().y < apexes.at(0).p->y))
			{
				apexes.push_back(Apex(&points.back()));
				edges.push_back(Edge(&apexes.back(), &apexes.at(0)));
				apexes.back().a = &edges.back();
			}
			else if (points.back().x > apexes.back().p->x 
				|| (points.back().x == apexes.back().p->x && points.back().y > apexes.back().p->y))
			{
				apexes.push_back(Apex(&points.back()));
				edges.push_back(Edge(&apexes.at(apexes.size() - 2), &apexes.back()));
				apexes.back().a = &edges.back();
			}
			else
			{
				for (int i = 0; i < edges.size(); ++i)
				{
					if ((points.back().x > edges.at(i).s1->p->x && points.back().x < edges.at(i).s2->p->x) 
						|| (points.back().x == edges.at(i).s1->p->x && points.back().y > edges.at(i).s1->p->y && points.back().y < edges.at(i).s2->p->y))
					{
						apexes.push_back(Apex(&points.back()));
						Apex* s2 = edges.at(i).s2;
						edges.at(i).s2 = &apexes.back();
						apexes.back().a = &edges.at(i);
						edges.push_back(Edge(&apexes.back(), s2));
						s2->a = &edges.back();
						break;
					}
				}
			}
		}
		else
		{
			apexes.push_back(Apex(&points.back()));

			for (int i = 0; i < apexes.size() - 1; ++i)
			{
				edges.push_back(Edge(&apexes.at(i), &apexes.back()));
			}
			apexes.back().a = &edges.back();

			for (int i = 0; i < (edges.size() - 1) / 2; ++i)
			{
				Edge *a1, *a2;
				Triangle t;
				a1 = &edges.at(i + (edges.size() - 1) / 2);
				a2 = &edges.at(i + (edges.size() - 1) / 2 + 1);

				t = Triangle(&edges.at(i), a1, a2);

				if (!isTriangleDirect(t))
				{
					t = Triangle(&edges.at(i), a2, a1);
				}

				triangles.push_back(t);
				addTriangleToItsEdges(&triangles.back());
			}
		}
	}
	else
	{
		bool isInTriangle = false, needToContinue = false;
		std::vector<Edge*> seen_edges;
		seen_edges.reserve(maxPoints * (maxPoints - 1) / 2);
		Edge* a;
		Circle c;
		Apex *s1, *s2, *s3;
		std::vector<Triangle>::iterator it1;
		std::vector<Edge>::iterator it2;
		Triangle td, tg;

		apexes.push_back(Apex(&points.back()));

		for (int i = 0; i < triangles.size(); ++i)
		{
			if (pointIsInTriangle(&triangles.at(i), &apexes.back()))
			{
				if (std::find(seen_edges.begin(), seen_edges.end(), triangles.at(i).a1) == seen_edges.end())
				{
					seen_edges.push_back(triangles.at(i).a1);
				}
				if (std::find(seen_edges.begin(), seen_edges.end(), triangles.at(i).a2) == seen_edges.end())
				{
					seen_edges.push_back(triangles.at(i).a2);
				}
				if (std::find(seen_edges.begin(), seen_edges.end(), triangles.at(i).a3) == seen_edges.end())
				{
					seen_edges.push_back(triangles.at(i).a3);
				}
				isInTriangle = true;
				break;
			}
		}

		if (!isInTriangle)
		{
			seen_edges = findSeenEdges(&apexes.back());
		}
		
		while (!seen_edges.empty())
		{
			a = seen_edges.back();
			seen_edges.pop_back();

			if (a->td != nullptr)
			{
				td = *a->td;
			}
			if (a->tg != nullptr)
			{
				tg = *a->tg;
			}

			if(a->td != nullptr)
			{
				std::tie(s1, s2, s3) = apexesOfTriangle(&td);
				c = circonscriptCircle(s1, s2, s3);

				if (sqrt(pow(c.center.x - apexes.back().p->x, 2) + pow(c.center.y - apexes.back().p->y, 2)) <= c.radius)
				{
					if (a == td.a1)
					{
						if (std::find(seen_edges.begin(), seen_edges.end(), td.a2) == seen_edges.end())
						{
							seen_edges.push_back(td.a2);
							//a->td->a2->td = nullptr;
						}
						if (std::find(seen_edges.begin(), seen_edges.end(), td.a3) == seen_edges.end())
						{
							seen_edges.push_back(td.a3);
							//a->td->a3->td = nullptr;
						}
					}
					else if (a == td.a2)
					{
						if (std::find(seen_edges.begin(), seen_edges.end(), td.a1) == seen_edges.end())
						{
							seen_edges.push_back(td.a1);
							//a->td->a1->td = nullptr;
						}
						if (std::find(seen_edges.begin(), seen_edges.end(), td.a3) == seen_edges.end())
						{
							seen_edges.push_back(td.a3);
							//a->td->a3->td = nullptr;
						}
					}
					else
					{
						if (std::find(seen_edges.begin(), seen_edges.end(), td.a1) == seen_edges.end())
						{
							seen_edges.push_back(td.a1);
							//a->td->a1->td = nullptr;
						}
						if (std::find(seen_edges.begin(), seen_edges.end(), td.a2) == seen_edges.end())
						{
							seen_edges.push_back(td.a2);
							//a->td->a2->td = nullptr;
						}
					}

					for (int i = 0; i < edges.size(); ++i)
					{
						if (&edges.at(i) == a)
						{
							continue;
						}
						if (edges.at(i).td == a->td)
						{
							edges.at(i).td = nullptr;
						}
						if (edges.at(i).tg == a->td)
						{
							edges.at(i).tg = nullptr;
						}
					}

					if (a->tg != nullptr)
					{
						if (a == tg.a1)
						{
							if (std::find(seen_edges.begin(), seen_edges.end(), tg.a2) == seen_edges.end())
							{
								seen_edges.push_back(tg.a2);
								//a->td->a2->td = nullptr;
							}
							if (std::find(seen_edges.begin(), seen_edges.end(), tg.a3) == seen_edges.end())
							{
								seen_edges.push_back(tg.a3);
								//a->td->a3->td = nullptr;
							}
						}
						else if (a == tg.a2)
						{
							if (std::find(seen_edges.begin(), seen_edges.end(), tg.a1) == seen_edges.end())
							{
								seen_edges.push_back(tg.a1);
								//a->td->a1->td = nullptr;
							}
							if (std::find(seen_edges.begin(), seen_edges.end(), tg.a3) == seen_edges.end())
							{
								seen_edges.push_back(tg.a3);
								//a->td->a3->td = nullptr;
							}
						}
						else
						{
							if (std::find(seen_edges.begin(), seen_edges.end(), tg.a1) == seen_edges.end())
							{
								seen_edges.push_back(tg.a1);
								//a->td->a1->td = nullptr;
							}
							if (std::find(seen_edges.begin(), seen_edges.end(), tg.a2) == seen_edges.end())
							{
								seen_edges.push_back(tg.a2);
								//a->td->a2->td = nullptr;
							}
						}

						for (int i = 0; i < edges.size(); ++i)
						{
							if (&edges.at(i) == a)
							{
								continue;
							}
							if (edges.at(i).td == a->tg)
							{
								edges.at(i).td = nullptr;
							}
							if (edges.at(i).tg == a->tg)
							{
								edges.at(i).tg = nullptr;
							}
						}
					}

					triangles.erase(std::remove(triangles.begin(), triangles.end(), *a->td), triangles.end());
					edges.erase(std::remove(edges.begin(), edges.end(), *a), edges.end());

					continue;
				}
			}
			if (a->tg != nullptr)
			{
				std::tie(s1, s2, s3) = apexesOfTriangle(&tg);
				c = circonscriptCircle(s1, s2, s3);

				if (sqrt(pow(c.center.x - apexes.back().p->x, 2) + pow(c.center.y - apexes.back().p->y, 2)) <= c.radius)
				{
					if (a == tg.a1)
					{
						if (std::find(seen_edges.begin(), seen_edges.end(), tg.a2) == seen_edges.end())
						{
							seen_edges.push_back(tg.a2);
							//a->tg->a2->tg = nullptr;
						}
						if (std::find(seen_edges.begin(), seen_edges.end(), tg.a3) == seen_edges.end())
						{
							seen_edges.push_back(tg.a3);
							//a->tg->a3->tg = nullptr;
						}
					}
					else if (a == tg.a2)
					{
						if (std::find(seen_edges.begin(), seen_edges.end(), tg.a1) == seen_edges.end())
						{
							seen_edges.push_back(tg.a1);
							//a->tg->a1->tg = nullptr;
						}
						if (std::find(seen_edges.begin(), seen_edges.end(), tg.a3) == seen_edges.end())
						{
							seen_edges.push_back(tg.a3);
							//a->tg->a3->tg = nullptr;
						}
					}
					else
					{
						if (std::find(seen_edges.begin(), seen_edges.end(), tg.a1) == seen_edges.end())
						{
							seen_edges.push_back(tg.a1);
							//a->tg->a1->tg = nullptr;
						}
						if (std::find(seen_edges.begin(), seen_edges.end(), tg.a2) == seen_edges.end())
						{
							seen_edges.push_back(tg.a2);
							//a->tg->a2->tg = nullptr;
						}
					}

					for (int i = 0; i < edges.size(); ++i)
					{
						if (&edges.at(i) == a)
						{
							continue;
						}
						if (edges.at(i).td == a->tg)
						{
							edges.at(i).td = nullptr;
						}
						if (edges.at(i).tg == a->tg)
						{
							edges.at(i).tg = nullptr;
						}
					}

					triangles.erase(std::remove(triangles.begin(), triangles.end(), *a->tg), triangles.end());
					edges.erase(std::remove(edges.begin(), edges.end(), *a), edges.end());

					continue;
				}
			}

			Edge cop = Edge(a->s1, a->s2);
			Edge* cop_p = &(*std::find(edges.begin(), edges.end(), cop));
			Edge *s4 = nullptr, *s5 = nullptr;
			bool s1Exists = false, s2Exists = false;

			for (int i = 0; i < edges.size(); ++i)
			{
				if (edges.at(i).s1 == cop_p->s1 || edges.at(i).s2 == cop_p->s1)
				{
					if (edges.at(i).s1 == &apexes.back() || edges.at(i).s2 == &apexes.back())
					{
						s4 = &edges.at(i);
						s1Exists = true;
					}
				}
				if (edges.at(i).s1 == cop_p->s2 || edges.at(i).s2 == cop_p->s2)
				{
					if (edges.at(i).s1 == &apexes.back() || edges.at(i).s2 == &apexes.back())
					{
						s5 = &edges.at(i);
						s2Exists = true;
					}
				}
			}

			if (!s1Exists)
			{
				edges.push_back(Edge(cop_p->s1, &apexes.back()));
				s4 = &edges.back();
			}

			if (!s2Exists)
			{
				edges.push_back(Edge(cop_p->s2, &apexes.back()));
				s5 = &edges.back();
			}

			apexes.back().a = &edges.back();

			Triangle tr = Triangle(cop_p, s5, s4);

			if (!isTriangleDirect(tr))
			{
				tr = Triangle(cop_p, s4, s5);
			}

			triangles.push_back(tr);
			addTriangleToItsEdges(&triangles.back());
		}
	}
}

void delaunayDeletePoint(Point* p)
{
	Apex s;

	for (int i = 0; i < apexes.size(); ++i)
	{
		if (apexes.at(i).p == p)
		{
			s = apexes.at(i);
			break;
		}
	}

	if (triangles.empty())
	{
		if (apexes.size() == 1)
		{
			apexes.clear();
			points.clear();
		}

		std::vector<Edge*> adjacent_edges = findAdjacentEdges(p);

		if (adjacent_edges.size() == 1)
		{
			edges.erase(std::find(edges.begin(), edges.end(), *adjacent_edges.at(0)));
			apexes.erase(std::find(apexes.begin(), apexes.end(), s));
		}
		else if (adjacent_edges.size() == 2)
		{
			Apex *s1, *s2;

			if (adjacent_edges.at(0)->s1 == &s)
			{
				s1 = adjacent_edges.at(0)->s2;
			}
			else
			{
				s1 = adjacent_edges.at(0)->s1;
			}

			if (adjacent_edges.at(1)->s1 == &s)
			{
				s2 = adjacent_edges.at(1)->s2;
			}
			else
			{
				s2 = adjacent_edges.at(1)->s1;
			}

			edges.erase(std::find(edges.begin(), edges.end(), *adjacent_edges.at(0)));
			edges.erase(std::find(edges.begin(), edges.end(), *adjacent_edges.at(1)));
			apexes.erase(std::find(apexes.begin(), apexes.end(), s));

			edges.push_back(Edge(s1, s2));
		}
	}
	else
	{
		std::vector<Edge*> adjacent_edges = findOrientedAdjacentEdges(p);
		std::vector<Triangle*> adjacent_triangles = findOrientedAdjacentTriangles(adjacent_edges);
		std::vector<Edge*> non_adjacent_edges = findNonAdjacentEdges(adjacent_edges, adjacent_triangles);

		std::map<Apex*, std::vector<Edge*>> adjacent_apexes;

		for (int i = 0; i < non_adjacent_edges.size(); ++i)
		{
			if (adjacent_apexes.find(non_adjacent_edges.at(i)->s1) == adjacent_apexes.end())//std::find(adjacent_apexes.begin(), adjacent_apexes.end(), non_adjacent_edges.at(i)->s1) == adjacent_apexes.end())
			{
				std::vector<Edge*> adj_edges;
				adj_edges.reserve(2);
				adj_edges.push_back(non_adjacent_edges.at(i));
				adjacent_apexes.insert(std::pair<Apex*, std::vector<Edge*>>(non_adjacent_edges.at(i)->s1, adj_edges));
			}
			else
			{
				adjacent_apexes.at(non_adjacent_edges.at(i)->s1).push_back(non_adjacent_edges.at(i));
			}

			if (adjacent_apexes.find(non_adjacent_edges.at(i)->s2) == adjacent_apexes.end())//std::find(adjacent_apexes.begin(), adjacent_apexes.end(), non_adjacent_edges.at(i)->s2) == adjacent_apexes.end())
			{
				std::vector<Edge*> adj_edges;
				adj_edges.reserve(2);
				adj_edges.push_back(non_adjacent_edges.at(i));
				adjacent_apexes.insert(std::pair<Apex*, std::vector<Edge*>>(non_adjacent_edges.at(i)->s2, adj_edges));
			}
			else
			{
				adjacent_apexes.at(non_adjacent_edges.at(i)->s2).push_back(non_adjacent_edges.at(i));
			}
		}

		apexes.erase(std::find(apexes.begin(), apexes.end(), s));

		for (int i = 0; i < adjacent_edges.size(); ++i)
		{
			edges.erase(std::find(edges.begin(), edges.end(), *adjacent_edges.at(i)));
		}

		for (int i = 0; i < adjacent_triangles.size(); ++i)
		{
			triangles.erase(std::find(triangles.begin(), triangles.end(), *adjacent_triangles.at(i)));
		}

		bool isConv, exists;
		Apex *s1, *s2;
		Edge a;
		Triangle tr;
		Circle c;
		
		if (non_adjacent_edges.back()->s1 == non_adjacent_edges.at(0)->s1
			|| non_adjacent_edges.back()->s1 == non_adjacent_edges.at(0)->s2
			|| non_adjacent_edges.back()->s2 == non_adjacent_edges.at(0)->s1
			|| non_adjacent_edges.back()->s2 == non_adjacent_edges.at(0)->s2)
		{

			while (non_adjacent_edges.size() > 3)
			{
				for (std::pair<Apex*, std::vector<Edge*>> pair : adjacent_apexes)
				{
					isConv = true;
					if (pair.second.at(0)->s1 == pair.first)
					{
						s1 = pair.second.at(0)->s2;
					}
					else
					{
						s1 = pair.second.at(0)->s1;
					}

					if (pair.second.at(1)->s1 == pair.first)
					{
						s2 = pair.second.at(1)->s2;
					}
					else
					{
						s2 = pair.second.at(1)->s1;
					}

					if (edgeExists(s1, s2))
					{
						continue;
					}

					a = Edge(s1, s2);
					tr = Triangle(&a, pair.second.at(0), pair.second.at(1));

					if (!isTriangleDirect(tr))
					{
						tr = Triangle(&a, pair.second.at(1), pair.second.at(0));
					}

					c = circonscriptCircle(pair.first, s1, s2);

					for (std::pair<Apex*, std::vector<Edge*>> pair2 : adjacent_apexes)
					{
						if (pair2.first == pair.first
							|| pair2.first == s1
							|| pair2.first == s2)
						{
							continue;
						}

						if (sqrt(pow(pair2.first->p->x - c.center.x , 2) + pow(pair2.first->p->y - c.center.y, 2)) < c.radius)
						{
							isConv = false;
							break;
						}
					}

					if (isConv)
					{
						for (Edge* ar : pair.second)
						{
							non_adjacent_edges.erase(std::find(non_adjacent_edges.begin(), non_adjacent_edges.end(), ar));
						}
						adjacent_apexes.erase(pair.first);
						break;
					}
				}

				edges.push_back(a);
				triangles.push_back(tr);
				non_adjacent_edges.push_back(&edges.back());
			}

			Triangle t = Triangle(non_adjacent_edges.at(0), non_adjacent_edges.at(1), non_adjacent_edges.at(2));

			if (!isTriangleDirect(t))
			{
				t = Triangle(non_adjacent_edges.at(0), non_adjacent_edges.at(2), non_adjacent_edges.at(1));
			}

			triangles.push_back(t);
			addTriangleToItsEdges(&triangles.back());
		}
		else
		{
			bool hasFoundApex = true;

			while (hasFoundApex)
			{
				hasFoundApex = false;

				for (std::pair<Apex*, std::vector<Edge*>> pair : adjacent_apexes)
				{
					isConv = true;

					if (pair.second.at(0)->s1 == pair.first)
					{
						s1 = pair.second.at(0)->s2;
					}
					else
					{
						s1 = pair.second.at(0)->s1;
					}

					if (pair.second.at(1)->s1 == pair.first)
					{
						s2 = pair.second.at(1)->s2;
					}
					else
					{
						s2 = pair.second.at(1)->s1;
					}
					
					if (edgeExists(s1, s2))
					{
						continue;
					}

					a = Edge(s1, s2);
					tr = Triangle(&a, pair.second.at(0), pair.second.at(1));

					if (!isTriangleDirect(tr))
					{
						tr = Triangle(&a, pair.second.at(1), pair.second.at(0));
					}

					c = circonscriptCircle(pair.first, s1, s2);

					for (std::pair<Apex*, std::vector<Edge*>> pair2 : adjacent_apexes)
					{
						if (pair2.first == pair.first
							|| pair2.first == s1
							|| pair2.first == s2)
						{
							continue;
						}

						if (sqrt(pow(pair2.first->p->x - c.center.x, 2) + pow(pair2.first->p->y - c.center.y, 2)) < c.radius)
						{
							isConv = false;
							break;
						}
					}

					if (isConv)
					{
						hasFoundApex = true;
						for (Edge* ar : pair.second)
						{
							non_adjacent_edges.erase(std::find(non_adjacent_edges.begin(), non_adjacent_edges.end(), ar));
						}
						adjacent_apexes.erase(pair.first);
						break;
					}
				}

				edges.push_back(a);
				triangles.push_back(tr);
				non_adjacent_edges.push_back(&edges.back());
			}
		}
	}
}

bool isTriangleDirect(Triangle tr)
{
	Apex *s1 = nullptr, *s2 = nullptr, *s3 = nullptr;

	std::tie(s1, s2, s3) = apexesOfTriangle(&tr);

	return determinant(s1, s2, s3) < 0;
}

bool isTriangleLeft(Triangle tr, Edge* e)
{
	Apex *s1 = nullptr, *s2 = nullptr, *s3 = nullptr;

	if (tr.a1 == e)
	{
		if (tr.a1->s1 == tr.a2->s1 || tr.a1->s2 == tr.a2->s1)
		{
			s1 = tr.a1->s1;
			s2 = tr.a1->s2;
			s3 = tr.a2->s2;
		}
		else if (tr.a1->s1 == tr.a2->s2 || tr.a1->s2 == tr.a2->s2)
		{
			s1 = tr.a1->s1;
			s2 = tr.a1->s2;
			s3 = tr.a2->s1;
		}
	}
	else if (tr.a2 == e)
	{
		if (tr.a2->s1 == tr.a3->s1 || tr.a2->s2 == tr.a3->s1)
		{
			s1 = tr.a2->s1;
			s2 = tr.a2->s2;
			s3 = tr.a3->s2;
		}
		else if (tr.a2->s1 == tr.a3->s2 || tr.a2->s2 == tr.a3->s2)
		{
			s1 = tr.a2->s1;
			s2 = tr.a2->s2;
			s3 = tr.a3->s1;
		}
	}
	else if (tr.a3 == e)
	{
		if (tr.a3->s1 == tr.a1->s1 || tr.a3->s2 == tr.a1->s1)
		{
			s1 = tr.a3->s1;
			s2 = tr.a3->s2;
			s3 = tr.a1->s2;
		}
		else if (tr.a3->s1 == tr.a1->s2 || tr.a3->s2 == tr.a1->s2)
		{
			s1 = tr.a3->s1;
			s2 = tr.a3->s2;
			s3 = tr.a1->s1;
		}
	}

	return determinant(s1, s2, s3) < 0;
}

float determinant(Apex* s1, Apex* s2, Apex* s3)
{
	return 0.5 * ((s1->p->x - s2->p->x) * (s3->p->y - s2->p->y) - (s1->p->y - s2->p->y) * (s3->p->x - s2->p->x));
}

bool delaunayCriteria(Edge *a)
{
	if (a->td == nullptr || a->tg == nullptr)
	{
		return true;
	}

	Point c;
	float rc;
	Apex *s1, *s2, *s3, *s4, *ts1, *ts2, *ts3;
	Circle circonscript;

	s1 = a->s1;
	s2 = a->s2;

	std::tie(ts1, ts2, ts3) = apexesOfTriangle(a->td);
	if (ts1 != s1 && ts1 != s2)
	{
		s3 = ts1;
	}
	else if (ts2 != s1 && ts2 != s2)
	{
		s3 = ts2;
	}
	else
	{
		s3 = ts3;
	}

	std::tie(ts1, ts2, ts3) = apexesOfTriangle(a->tg);
	if (ts1 != s1 && ts1 != s2)
	{
		s4 = ts1;
	}
	else if (ts2 != s1 && ts2 != s2)
	{
		s4 = ts2;
	}
	else
	{
		s4 = ts3;
	}

	//Pour td
	circonscript = circonscriptCircle(s1, s2, s3);

	if (sqrt(pow(s4->p->x - circonscript.center.x, 2) + pow(s4->p->y - circonscript.center.y, 2)) <= circonscript.radius)
	{
		return false;
	}

	//Pour tg
	circonscript = circonscriptCircle(s1, s2, s4);

	if (sqrt(pow(s3->p->x - circonscript.center.x, 2) + pow(s3->p->y - circonscript.center.y, 2)) <= circonscript.radius)
	{
		return false;
	}

	return true;
}

std::tuple<Apex*, Apex*, Apex*> apexesOfTriangle(Triangle* tr)
{
	Apex *s1, *s2, *s3;

	if (tr->a1->s1 == tr->a2->s1)
	{
		s1 = tr->a1->s1;
		s2 = tr->a2->s2;
		s3 = tr->a1->s2;
	}
	else if (tr->a1->s1 == tr->a2->s2)
	{
		s1 = tr->a1->s1;
		s2 = tr->a2->s1;
		s3 = tr->a1->s2;
	}
	else if (tr->a1->s2 == tr->a2->s1)
	{
		s1 = tr->a1->s2;
		s2 = tr->a2->s2;
		s3 = tr->a1->s1;
	}
	else if (tr->a1->s2 == tr->a2->s2)
	{
		s1 = tr->a1->s2;
		s2 = tr->a2->s1;
		s3 = tr->a1->s1;
	}

	return std::make_tuple(s1, s2, s3);
}

Circle circonscriptCircle(Apex* s1, Apex* s2, Apex* s3)
{
	Circle c;

	float x1, x2, y1, y2;
	float a1, a2, b1, b2;

	x1 = (s1->p->x + s2->p->x) / 2;
	y1 = (s1->p->y + s2->p->y) / 2;
	x2 = (s1->p->x + s3->p->x) / 2;
	y2 = (s1->p->y + s3->p->y) / 2;

	a1 = (s1->p->y - s2->p->y) / (s1->p->x - s2->p->x);
	a2 = (s1->p->y - s3->p->y) / (s1->p->x - s3->p->x);

	b1 = y1 + 1 / a1 * x1;
	b2 = y2 + 1 / a2 * x2;

	c.center.x = (b2 - b1) / (1 / a2 - 1 / a1);
	c.center.y = -1 / a1 * c.center.x + b1;
	
	c.radius = sqrt(pow(s1->p->x - c.center.x, 2) + pow(s1->p->y - c.center.y, 2));

	return c;
}

bool pointIsInTriangle(Triangle* t, Apex* s)
{
	float d, dref;
	Apex *s1, *s2, *s3;
	std::tie(s1, s2, s3) = apexesOfTriangle(t);

	if (t->a1->s1 != s1 && t->a1->s2 != s1)
	{
		dref = (s1->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
			- (s1->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else if (t->a1->s1 != s2 && t->a1->s2 != s2)
	{
		dref = (s2->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
			- (s2->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else
	{
		dref = (s3->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
			- (s3->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}

	d = (s->p->x - t->a1->s1->p->x) * (t->a1->s2->p->y - t->a1->s1->p->y)
		- (s->p->y - t->a1->s1->p->y) * (t->a1->s2->p->x - t->a1->s1->p->x);
	d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

	if (d != dref && d != 0)
	{
		return false;
	}

	if (d == 0)
	{
		return true;
	}

	if (t->a2->s1 != s1 && t->a2->s2 != s1)
	{
		dref = (s1->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
			- (s1->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else if (t->a2->s1 != s2 && t->a2->s2 != s2)
	{
		dref = (s2->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
			- (s2->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else
	{
		dref = (s3->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
			- (s3->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}

	d = (s->p->x - t->a2->s1->p->x) * (t->a2->s2->p->y - t->a2->s1->p->y)
		- (s->p->y - t->a2->s1->p->y) * (t->a2->s2->p->x - t->a2->s1->p->x);
	d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

	if (d != dref && d != 0)
	{
		return false;
	}

	if (d == 0)
	{
		return true;
	}

	if (t->a3->s1 != s1 && t->a3->s2 != s1)
	{
		dref = (s1->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
			- (s1->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else if (t->a3->s1 != s2 && t->a3->s2 != s2)
	{
		dref = (s2->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
			- (s2->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}
	else
	{
		dref = (s3->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
			- (s3->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;
	}

	d = (s->p->x - t->a3->s1->p->x) * (t->a3->s2->p->y - t->a3->s1->p->y)
		- (s->p->y - t->a3->s1->p->y) * (t->a3->s2->p->x - t->a3->s1->p->x);
	d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

	if (d != dref && d != 0)
	{
		return false;
	}

	return true;
}

std::vector<Edge*> findSeenEdges(Apex* s)
{
	Triangle* tref;
	std::vector<Apex*> tr_apexes;
	tr_apexes.reserve(2);
	Apex a;
	std::vector<Edge*> seen_edges;
	seen_edges.reserve(edges.size());
	float d, dref;

	for (int k = 0; k < edges.size(); ++k)
	{
		//if both triangle non null => continue
		if (edges.at(k).td != nullptr
			&& edges.at(k).tg != nullptr)
		{
			continue;
		}

		//find non null triangle
		tref = (edges.at(k).td != nullptr) ? edges.at(k).td : edges.at(k).tg;

		//add apexes of edge
		tr_apexes.push_back(edges.at(k).s1);
		tr_apexes.push_back(edges.at(k).s2);

		//find apex of triangle not in edge
		if (tref->a1 != &edges.at(k))
		{
			if (std::find(tr_apexes.begin(), tr_apexes.end(), tref->a1->s1)
				!= tr_apexes.end())
			{
				a = *tref->a1->s2;
			}
			else
			{
				a = *tref->a1->s1;
			}
		}
		else if (tref->a2 != &edges.at(k))
		{
			if (std::find(tr_apexes.begin(), tr_apexes.end(), tref->a2->s1)
				!= tr_apexes.end())
			{
				a = *tref->a2->s2;
			}
			else
			{
				a = *tref->a2->s1;
			}
		}

		//calculate reference value for side of edge
		dref = (a.p->x - edges.at(k).s1->p->x) * (edges.at(k).s2->p->y - edges.at(k).s1->p->y)
			- (a.p->y - edges.at(k).s1->p->y) * (edges.at(k).s2->p->x - edges.at(k).s1->p->x);
		dref = (dref < 0) ? -1 : (dref > 0) ? 1 : 0;

		//calculate real value for side of edge
		d = (s->p->x - edges.at(k).s1->p->x) * (edges.at(k).s2->p->y - edges.at(k).s1->p->y)
			- (s->p->y - edges.at(k).s1->p->y) * (edges.at(k).s2->p->x - edges.at(k).s1->p->x);
		d = (d < 0) ? -1 : (d > 0) ? 1 : 0;

		//check if edge is seen by point
		if (d != dref && d != 0)
		{
			seen_edges.push_back(&edges.at(k));
		}

		tr_apexes.clear();
	}

	return seen_edges;
}

std::vector<Edge*> findAdjacentEdges(Point* p)
{
	std::vector<Edge*> adjacent_edges;
	adjacent_edges.reserve(edges.size());

	for (int i = 0; i < edges.size(); ++i)
	{
		if (edges.at(i).s1->p == p || edges.at(i).s2->p == p)
		{
			adjacent_edges.push_back(&edges.at(i));
		}
	}

	return adjacent_edges;
}

float orientedAngle(Edge* a1, Edge* a2, Point* p)
{
	Apex *s = nullptr, *s1 = nullptr, *s2 = nullptr;

	for (int i = 0; i < apexes.size(); ++i)
	{
		if (apexes.at(i).p == p)
		{
			s = &apexes.at(i);
			continue;
		}
		if (apexes.at(i).p == a1->s1->p)
		{
			s1 = &apexes.at(i);
			continue;
		}
		if (apexes.at(i).p == a1->s2->p)
		{
			s1 = &apexes.at(i);
			continue;
		}
		if (apexes.at(i).p == a2->s1->p)
		{
			s2 = &apexes.at(i);
			continue;
		}
		if (apexes.at(i).p == a2->s2->p)
		{
			s2 = &apexes.at(i);
			continue;
		}
	}

	float angle = atan2(s2->p->y - s->p->y, s2->p->x - s->p->x) - atan2(s1->p->y - s->p->y, s1->p->x - s->p->x);
	if (angle < 0)
	{
		angle += 2 * PI;
	}

	return angle;
}

std::vector<Edge*> findOrientedAdjacentEdges(Point* p)
{
	std::map<Edge*, float> oriented_angles;
	std::vector<Edge*> adjacent_edges = findAdjacentEdges(p);
	adjacent_edges.reserve(edges.size());
	Edge* aref = adjacent_edges.at(0);

	for (int i = 1; i < adjacent_edges.size(); ++i)
	{
		oriented_angles.insert(std::pair<Edge*, float>(adjacent_edges.at(i), orientedAngle(aref, adjacent_edges.at(i), p)));
	}

	typedef std::function<bool(std::pair<Edge*, float>, std::pair<Edge*, float>)> Comparator;

	Comparator compFunctor = [](std::pair<Edge*, float> elem1, std::pair<Edge*, float> elem2)
	{
		return elem1.second < elem2.second;
	};

	std::set<std::pair<Edge*, float>, Comparator> setOfAngles(oriented_angles.begin(), oriented_angles.end(), compFunctor);

	adjacent_edges.clear();
	adjacent_edges.push_back(aref);
	for (std::pair<Edge*, float> element : setOfAngles)
	{
		adjacent_edges.push_back(element.first);
	}

	return adjacent_edges;
}

std::vector<Triangle*> findOrientedAdjacentTriangles(std::vector<Edge*> adjacent_edges)
{
	std::vector<Triangle*> adjacent_triangles;
	adjacent_triangles.reserve(adjacent_edges.size());

	std::vector<Edge*>::iterator it1;
	std::vector<Edge*>::iterator it2;
	std::vector<Edge*>::iterator it3;

	for (int i = 0; i < adjacent_edges.size(); ++i)
	{
		for (int j = 0; j < triangles.size(); ++j)
		{
			it1 = std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(j).a1);
			it2 = std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(j).a2);
			it3 = std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(j).a3);

			if ((it1 == adjacent_edges.end() && it2 == adjacent_edges.end())
				|| (it1 == adjacent_edges.end() && it3 == adjacent_edges.end())
				|| (it2 == adjacent_edges.end() && it3 == adjacent_edges.end()))
			{
				continue;
			}

			if (std::find(adjacent_triangles.begin(), adjacent_triangles.end(), &triangles.at(j)) == adjacent_triangles.end())
			{
				adjacent_triangles.push_back(&triangles.at(j));
			}
			break;
		}
	}

	return adjacent_triangles;
}

std::vector<Edge*> findNonAdjacentEdges(std::vector<Edge*> adjacent_edges, std::vector<Triangle*> adjacent_triangles)
{
	std::vector<Edge*> non_adjacent_edges;
	non_adjacent_edges.reserve(edges.size());

	for (int i = 0; i < triangles.size(); ++i)
	{
		if (std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(i).a1) == adjacent_edges.end())
		{
			non_adjacent_edges.push_back(triangles.at(i).a1);
			continue;
		}

		if (std::find(adjacent_edges.begin(), adjacent_edges.end(), triangles.at(i).a2) == adjacent_edges.end())
		{
			non_adjacent_edges.push_back(triangles.at(i).a2);
			continue;
		}
			
		non_adjacent_edges.push_back(triangles.at(i).a3);
	}

	return non_adjacent_edges;
}

void addTriangleToItsEdges(Triangle* t)
{
	if (isTriangleLeft(*t, t->a1))
	{
		t->a1->tg = t;
	}
	else
	{
		t->a1->td = t;
	}

	if (isTriangleLeft(*t, t->a2))
	{
		t->a2->tg = t;
	}
	else
	{
		t->a2->td = t;
	}

	if (isTriangleLeft(*t, t->a3))
	{
		t->a3->tg = t;
	}
	else
	{
		t->a3->td = t;
	}
}

bool edgeExists(Apex* s1, Apex* s2)
{
	for (Edge edge : edges)
	{
		if ((edge.s1 == s1 && edge.s2 == s2)
			|| (edge.s1 == s2 && edge.s2 == s1))
		{
			return true;
		}
	}

	return false;
}