#include "rect.h"
#include "amath.h"
#include <QDebug>
#include <QPainter>
#include <cmath>
#include <functional>

Rect::Rect(QPointF p, float width, float height)
{
    points.append(p);
    points.append(QPointF(p.x() + width, p.y()));
    points.append(QPointF(p.x() + width, p.y() + height));
    points.append(QPointF(p.x(), p.y() + height));
}

Rect::Rect(QPointF end, QPointF start, float height)
{
    QPointF v0_width = amath::vector(end, start);
    float halfHeight = height / 2;
    QPointF v90 = amath::normal(amath::unit(v0_width));
    QPointF v90_halfHeight = amath::multi(v90, halfHeight);
    QPointF p1 = amath::sum(start, v90_halfHeight);
    QPointF p2 = amath::sum(p1, v0_width);
    QPointF v270_height = amath::multi(amath::negative(v90), height);
    QPointF p3 = amath::sum(p2, v270_height);
    QPointF v180_width = amath::negative(v0_width);
    QPointF p4 = amath::sum(p3, v180_width);
    points = QVector<QPointF>({p1, p2, p3, p4});
}

bool Rect::contains(QPointF p)
{
    QPointF p1 = points[0];
    QPointF p2 = points[1];
    QPointF p4 = points[3];
    QPointF vec1 = amath::vector(p2, p1);
    QPointF vec2 = amath::vector(p4, p1);
    QPointF vecP = amath::vector(p, p1);
    const float dot1 = amath::dotProduct(vec1, vec1);
    const float dot2 = amath::dotProduct(vec2, vec2);
    const float dotIs1 = amath::dotProduct(vec1, vecP);
    const float dotIs2 = amath::dotProduct(vec2, vecP);

    return (dot1 > dotIs1 && dotIs1 > 0) &&
            (dot2 > dotIs2 && dotIs2 > 0);
}

float Rect::rotationAngle()
{
    QPointF p1 = points[0];
    QPointF p2 = points[1];
    QPointF v = amath::vector(p2, p1);
    float len = amath::calcLength(v);
    float angle = std::acos(float(v.x()) / len);
    float dot = amath::dotProduct(v, QPointF(0, 1));
    return dot > 0 ? angle : -angle;
}

QPointF Rect::center()
{
    return QPointF(
                (points[0].x() + points[2].x()) / 2,
                (points[0].y() + points[2].y()) / 2
            );
}

float Rect::width()
{
    QPointF vector = amath::vector(points[1], points[0]);
    return amath::calcLength(vector);
}

float Rect::height()
{
    QPointF vector = amath::vector(points[2], points[1]);
    return amath::calcLength(vector);
}

void Rect::forEachPoint(std::function<void (QPointF)> func)
{
    std::for_each(points.begin(), points.end(), func);
}
