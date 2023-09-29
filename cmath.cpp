#include "cmath.h"
#include <cmath>

bool CMath::isContainsRect(QPointF point, QPointF p4, QPointF p1, QPointF p2)
{
    QPointF vec_1 = QPointF(p2.x() - p1.x(), p2.y() - p1.y());
    QPointF vec_2 = QPointF(p4.x() - p1.x(), p4.y() - p1.y());
    QPointF vec_is = QPointF(point.x() - p1.x(), point.y() - p1.y());
    const float dot_1 = vec_1.x() * vec_1.x() + vec_1.y() * vec_1.y();
    const float dot_2 = vec_2.x() * vec_2.x() + vec_2.y() * vec_2.y();
    const float dot_is_1 = vec_is.x() * vec_1.x() + vec_is.y() * vec_1.y();
    const float dot_is_2 = vec_is.x() * vec_2.x() + vec_is.y() * vec_2.y();

    return (dot_1 > dot_is_1 && dot_is_1 > 0) &&
            (dot_2 > dot_is_2 && dot_is_2 > 0);
}

bool CMath::isContainsCircle(const QPointF &p, const QPointF &c, const float r)
{
    QPointF vec = QPointF(p.x() - c.x(), p.y() - c.y());
    float l = std::sqrt(vec.x() * vec.x() + vec.y() * vec.y());
    return l <= r;
}

QPointF CMath::findUnitVector(QPointF p1, QPointF p2)
{
    float dx = p2.x() - p1.x();
    float dy = p2.y() - p1.y();
    float len = calcLength(p1, p2);
    return QPointF(dx / len, dy / len);
}

QPointF CMath::findUnitVector(QPointF v)
{
    float len = calcLength(v, QPointF(0, 0));
    return QPointF(v.x() / len, v.y() / len);
}

QVector2D CMath::calcNormal(QPointF p1, QPointF p2)
{
    QVector2D vector(p2.x() - p1.x(), p2.y() - p1.y());
    return QVector2D(rotate90N(vector.toPointF()));
}

float CMath::calcLength(QPointF p1, QPointF p2)
{
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    return float(std::sqrt(dx * dx + dy * dy));
}

float CMath::dotProduct(QPointF p1, QPointF p2)
{
    return p1.x() * p2.x() + p1.y() * p2.y();
}

QPointF CMath::rotate90N(QPointF p)
{
    return QPointF(p.y(), -p.x());
}

QPointF CMath::multiPoint(float multi, QPointF p)
{
    return QPointF(multi * p.x(), multi * p.y());
}

QPointF CMath::rotate45(QPointF p)
{
    const float pi = 3.14159265358979323846;
    const float sin = std::sin(pi / 4);
    const float cos = std::cos(pi / 4);

    return QPointF(p.x() * cos - p.y() * sin, p.x() * sin + p.y() * cos);
}

QPointF CMath::rotate45N(QPointF p)
{
    const float pi = 3.14159265358979323846;
    const float sin = std::sin(-pi / 4);
    const float cos = std::cos(-pi / 4);

    return QPointF(p.x() * cos - p.y() * sin, p.x() * sin + p.y() * cos);
}

QPointF CMath::calcVector(QPointF p1, QPointF p2) {
    return QPointF(p2.x() - p1.x(), p2.y() - p1.y());
}


QPointF CMath::sumPoint(QPointF p1, QPointF p2)
{
    return QPointF(p1.x() + p2.x(), p1.y() + p2.y());
}

QPointF CMath::rotate90(QPointF p)
{
    return QPointF(-p.y(), p.x());
}
