#ifndef DEF_SQUAREMATRICE
#define DEF_SQUAREMATRICE

#include "CVecteur.h"

class CQuaternion;

class SquareMatrice
{
public:

    SquareMatrice(unsigned int normeMatrice, bool isIdentity);
    ~SquareMatrice();
    float* get_ptr();
    int const getNorme();
    void mult(SquareMatrice const *matrice2);
    void print();
    void reinit(bool isIdentity);
    static void print(const float* matrix, int norme);
    static void mult(const float* ma1, const float* m2,int norme,float* result);
    void mult(const float* m2,int norme);

private:
	unsigned int m_norme;
    float *m_matrice;
    void allocMatrice();

};
#endif
