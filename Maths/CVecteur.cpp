#include "CVecteur.h"
#include <cmath> 

using namespace std;

CVecteur::CVecteur(float x, float y)
{
    m_dX = x;
    m_dY = y;
}
CVecteur::CVecteur()
{
}

CVecteur::CVecteur(maths::Point p1, maths::Point p2)
{
	m_p1 = p1;
	m_p2 = p2;

	m_dX = (p2.x - p1.x) / (p2.y - p1.y);
	m_dY = (p2.y - p1.y) / (p2.x - p1.x);
}

CVecteur::~CVecteur()
{

}

// Début accesseurs
float CVecteur::getX()
{
    return m_dX;
}

float CVecteur::getY()
{
    return m_dY;
}

void CVecteur::setX(float x)
{
    m_dX=x;
}

void CVecteur::setY(float y)
{
    m_dY=y;
}

maths::Point CVecteur::Intersection(CVecteur vecteur)
{
	OrganizePoint();
	vecteur.OrganizePoint();

	maths::Point pointError;
	pointError.x = pointError.y = -1.0f;
	// Calcul des deux équations de droite
	float coeffDir1 = (m_p2.y - m_p1.y) / (m_p2.x - m_p1.x);
	float coeffDir2 = (vecteur.m_p2.y - vecteur.m_p1.y) / (vecteur.m_p2.x - vecteur.m_p1.x);

	cout << coeffDir1 << "  " << coeffDir2 << endl;
	if (coeffDir1 == coeffDir2)
	{
		return pointError;
	}

	float b1 = m_p1.y - coeffDir1*m_p1.x;
	float b2 = vecteur.m_p1.y - coeffDir2*vecteur.m_p1.x;
	maths::Point p;
	p.x = -((b1 - b2) / (coeffDir1 - coeffDir2));
	p.y = coeffDir1 * p.x + b1;

	if (p.x < m_p1.x || p.x > m_p2.x || p.x < vecteur.m_p1.x || p.x > vecteur.m_p2.x)
	{
		return pointError;
	}

	return p;
}


maths::Point CVecteur::Intersection(maths::Point p1, maths::Point p2, maths::Point p3, maths::Point p4)
{

	//cout << p1.x << "  " << p1.y << "        " << p2.x << "  " << p2.y << endl;

	CVecteur::OrganizePoint(&p1, &p2);
	CVecteur::OrganizePoint(&p3, &p4);


	
	maths::Point pointError;
	pointError.x = pointError.y = -1.0f;
	// Calcul des deux équations de droite
	float coeffDir1 = (p2.y - p1.y) / (p2.x - p1.x);
	float coeffDir2 = (p4.y - p3.y) / (p4.x - p3.x);
	
	//cout << coeffDir1 << "  " << coeffDir2 << endl;
	if (coeffDir1 == coeffDir2)
	{
		//cout << "C DLA MERDE" << endl;
		return pointError;
	}

	float b1 = p1.y - coeffDir1*p1.x;
	float b2 = p3.y - coeffDir2*p3.x;
	maths::Point p;
	p.x = -((b1 - b2) / (coeffDir1 - coeffDir2));
	p.y = coeffDir1 * p.x + b1;
	
	if(p.x < p1.x || p.x > p2.x || p.x < p3.x || p.x > p4.x )
	{ 
		return pointError;
	}

	return p;
}


void CVecteur::OrganizePoint(maths::Point* p1, maths::Point* p2)
{
	if (p1->x > p2->x)
	{
		float tmp = p1->x;
		p1->x = p2->x;
		p2->x = tmp;

		tmp = p1->y;
		p1->y = p2->y;
		p2->y = tmp;

	}
}


void CVecteur::OrganizePoint()
{
	if (m_p1.x > m_p2.x)
	{
		float tmp = m_p1.x;
		m_p1.x = m_p2.x;
		m_p2.x = tmp;

		tmp = m_p1.y;
		m_p1.y = m_p2.y;
		m_p2.y = tmp;

	}
}

