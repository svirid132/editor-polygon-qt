#ifndef POINT_H
#define POINT_H

#include <QPointF>
#include <cmath>
#include "amath.h"

class Point
{
public:
    Point(QPointF c, float r);
    bool contains(const QPointF &p);
    float radius();
    void setRadius(float r);

public:
    QPointF c; // центер окружности
    float r; // радиус окружности
};

#endif // POINT_H
