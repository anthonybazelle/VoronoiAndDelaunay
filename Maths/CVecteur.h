#ifndef CVECTEUR_H
#define CVECTEUR_H

#include <math.h>
#include "Polygon.h"
#include <iostream>

class CVecteur
{
    public:
        CVecteur(float x, float y);
		CVecteur(maths::Point p1, maths::Point p2);
		CVecteur();
        virtual ~CVecteur();
        float getX();
        float getY();
        void setX(float x);
        void setY(float y);
		static maths::Point Intersection(maths::Point p1, maths::Point p2, maths::Point p3, maths::Point p4);
		maths::Point Intersection(CVecteur vecteur);
		static void OrganizePoint(maths::Point* p1, maths::Point* p2);
		void OrganizePoint();

	protected:
    private:
        float m_dX;
        float m_dY;
        float m_dZ;
		maths::Point m_p1;
		maths::Point m_p2;
};

#endif // CVECTEUR_H
