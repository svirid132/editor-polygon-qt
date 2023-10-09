#include "amath.h"
#include <cmath>

QPointF amath::vector(QPointF end, QPointF begin) {
    return QPointF(end.x() - begin.x(), end.y() - begin.y());
}

float amath::dotProduct(QPointF v1, QPointF v2) {
    return v1.x() * v2.x() + v1.y() * v2.y();
}

float amath::calcLength(QPointF v) {
    return std::sqrt(v.x() * v.x() + v.y() * v.y());
}

QPointF amath::unit(QPointF v)
{
    float len = calcLength(v);
    return QPointF (v.x() / len, v.y() / len);
}

QPointF amath::normal(QPointF v)
{
    return QPointF(-v.y(), v.x());
}

QPointF amath::multi(QPointF v, float multi)
{
    return QPointF(v.x() * multi, v.y() * multi);
}

QPointF amath::negative(QPointF v)
{
    return QPointF(-v.x(), -v.y());
}

QPointF amath::sum(QPointF v1, QPointF v2)
{
    return QPointF(v1.x() + v2.x(), v1.y() + v2.y());
}
