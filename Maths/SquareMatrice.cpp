#include "SquareMatrice.h"
#include <cmath>

using namespace std;

SquareMatrice::SquareMatrice(unsigned int normeMatrice, bool isIdentity): m_norme((int)normeMatrice)
{
    allocMatrice();
    this->reinit(isIdentity);

}

void SquareMatrice::reinit(bool isIdentity)
{
    for(int i=0;i<m_norme;i++)
    {
        for(int j=0;j<m_norme;j++)
        {
            if(isIdentity && i==j)
                m_matrice[i*m_norme+j]=1;
            else
                m_matrice[i*m_norme+j]=0;
        }
    }
}

void SquareMatrice::allocMatrice()
{
    m_matrice = (float*) malloc(sizeof(float)*m_norme*m_norme);
}

SquareMatrice::~SquareMatrice()
{
    for(int i=0;i<m_norme;i++)
    {
        free(m_matrice);
    }
}


float* SquareMatrice::get_ptr()
{
    return m_matrice;
}

int const SquareMatrice::getNorme()
{
    return m_norme;
}

void SquareMatrice::mult(SquareMatrice const *matrice2)
{
    if(m_norme != matrice2->m_norme)
    {
        cout<<"Multiplication entre matrice de norme différentes"<<endl;
        return;
    }

    float* matrix = (float*) malloc(sizeof(float)*m_norme*m_norme);

    for(int i=0;i<m_norme;i++)
    {
        for(int j=0;j<m_norme;j++)
        {
            float total = 0;
            for(int k=0;k<m_norme;k++)
            {
                total+=m_matrice[i*m_norme+k]*matrice2->m_matrice[k*m_norme+j];
            }
            matrix[i*m_norme+j]=total;
        }
    }
//    free(m_matrice);
    m_matrice=matrix;
}

void SquareMatrice::mult(const float* ma1, const float* m2,int norme,float* result)
{
    for(int i=0;i<norme;i++)
    {
        for(int j=0;j<norme;j++)
        {
            float total = 0;
            for(int k=0;k<norme;k++)
            {
                total+=ma1[i*norme+k]*m2[k*norme+j];
            }
            result[i*norme+j]=total;
        }
    }
}

void SquareMatrice::mult(const float* m2,int norme)
{
    if(m_norme != norme)
    {
        cout<<"Multiplication entre matrice de norme différentes"<<endl;
        return;
    }
    float *result = (float*) malloc(norme*norme*sizeof(float));
    for(int i=0;i<norme;i++)
    {
        for(int j=0;j<norme;j++)
        {
            float total = 0;
            for(int k=0;k<norme;k++)
            {
                total+=m_matrice[i*norme+k]*m2[k*norme+j];
            }
            result[i*norme+j]=total;
        }
    }
    for(int i=0;i<norme;i++)
    {
        for(int j=0;j<norme;j++)
        {
            m_matrice[i*norme+j]=result[i*norme+j];
        }
    }
    free(result);
}

void SquareMatrice::print()
{
    SquareMatrice::print(m_matrice, m_norme);
}

void SquareMatrice::print(const float* matrix, int norme)
{
    for(int i=0;i<norme;i++)
    {
        cout<<"|";
        for(int j=0;j<norme;j++)
        {
            cout<<" "<<matrix[i*norme+j];
        }
        cout<<" |"<<endl;
    }
}


