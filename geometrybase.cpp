#include "geometrybase.h"
#include <QPainter>
#include "amath.h"
#include <QDebug>
#include <cmath>

void GeometryBase::rotate(float angle, QPointF origin)
{
    const float sin = std::sin(angle);
    const float cos = std::cos(angle);
    amath::Matrix m = amath::Matrix(1, 0, 0, 1, origin.x(), origin.y());
    m = m.multi(amath::Matrix(cos, sin, -sin, cos, 0, 0));
    m = m.multi(amath::Matrix(1, 0, 0, 1, -origin.x(), -origin.y()));
    QVector<QPointF> points;
    std::transform(this->points.begin(), this->points.end(),
                           std::back_inserter(points),
                           [&m](const QPointF& p) { return m.multi(p); } );
    this->points = points;
}

void GeometryBase::translate(QPointF v)
{
    QVector<QPointF> points;
    std::transform(this->points.begin(), this->points.end(),
                   std::back_inserter(points),
                   [=](const QPointF& p) { return QPointF(p.x() + v.x(), p.y() + v.y()); } );
    this->points = points;
}

void GeometryBase::scale(float Sx, float Sy, QPointF origin, float angle)
{
    const float sin = std::sin(angle);
    const float cos = std::cos(angle);
    amath::Matrix m = amath::Matrix(1, 0, 0, 1, origin.x(), origin.y());
    m = m.multi(amath::Matrix(cos, sin, -sin, cos, 0, 0));
    m = m.multi(amath::Matrix(Sx, 0, 0, Sy, 0, 0));
    m = m.multi(amath::Matrix(cos, -sin, sin, cos, 0, 0));
    m = m.multi(amath::Matrix(1, 0, 0, 1, -origin.x(), -origin.y()));
    QVector<QPointF> points;
    std::transform(this->points.begin(), this->points.end(),
                           std::back_inserter(points),
                           [&m](const QPointF& p) { return m.multi(p); } );
    this->points = points;
}

void GeometryBase::draw(QPainter *painter)
{
    painter->drawPolygon(points.data(), points.length());
}

void GeometryBase::draw(std::function<void(const QVector<QPointF>&)> func) {
    func(points);
}

void GeometryBase::zoom(GeometryBase *base, float contentX, float contentY, float scale)
{
    QVector<QPointF> points;
    std::transform(base->points.begin(), base->points.end(),
                   std::back_inserter(points),
                   [=](const QPointF& p) { return QPointF(contentX + p.x() * scale,
                                                         contentY + p.y() * scale); } );
    base->points = points;
}

float GeometryBase::width()
{
    float leftX = 9000000;
    float rightX = -9000000;
    std::for_each(points.begin(), points.end(), [&leftX, &rightX](QPointF p) {
        if (leftX > p.x()) {
            leftX = p.x();
        }
        if (rightX < p.x()) {
            rightX = p.x();
        }
    });

    return rightX - leftX;
}

float GeometryBase::height()
{
    float topY = -9000000;
    float bottomY = 9000000;
    std::for_each(points.begin(), points.end(), [&topY, &bottomY](QPointF p) {
        if (topY < p.y()) {
            topY = p.y();
        }
        if (bottomY > p.y()) {
            bottomY = p.y();
        }
    });

    return topY - bottomY;
}

int GeometryBase::size() {
    return points.size();
}

QPointF GeometryBase::operator[](int index)
{
    int ind = index % points.size();
    return points[ind];
}
