#ifndef MATRIX_H
#define MATRIX_H

#include <QPointF>

class Matrix
{
public:
    Matrix(float a, float b, float c, float d, float e, float f);
    QPointF multiVector(QPointF p);

private:
    float a;
    float b;
    float c;
    float d;
    float e;
    float f;
};

#endif // MATRIX_H
