#include "matrix.h"

Matrix::Matrix(float a, float b, float c, float d, float e, float f)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    this->e = e;
    this->f = f;
}

QPointF Matrix::multiVector(QPointF p)
{
    return QPointF(
                this->a * float(p.x()) + this->c * float(p.y()) + this->e,
                this->b * float(p.x()) + this->d * float(p.y()) + this->f
                );
}
