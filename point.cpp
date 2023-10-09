#include "point.h"

#include <QDebug>

Point::Point(QPointF c, float r)
{
    this->c = c;
    this->r = r;
}

void Point::setRadius(float r) {
    this->r = r;
}

bool Point::contains(const QPointF &p)
{
    QPointF v = amath::vector(p, c);
    float l = amath::calcLength(v);
    return l <= r;
}

float Point::radius()
{
    return r;
}
