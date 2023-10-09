#include "polygon.h"

#include <QDebug>
#include <QPainter>
#include <QPen>

Polygon::Polygon(const QVector<QPointF>& points)
{
    this->points = points;
}

QVector<QPointF> Polygon::getPoints()
{
    return this->points;
}

QPointF Polygon::leftBottomPoint()
{
    QPointF point = QPointF(9000000, 9000000);
    std::for_each(points.begin(), points.end(), [&point](QPointF p) {
        if (point.x() > p.x()) {
            point.setX(p.x());
        }
        if (point.y() > p.y()) {
            point.setY(p.y());
        }
    });

    return point;
}

void Polygon::draw(QPainter* painter, int selectedIndex, Mode mode)
{
    painter->save();
    QPen pen;
    int penWidth = 2;
    pen.setWidth(penWidth);
    QColor defaultColor = "white";
    QColor selectedColor = "#FFC700";
    pen.setColor(QColor(defaultColor));
    painter->setPen(pen);
    painter->drawPolygon(points.data(), points.length());
    painter->restore();

    painter->save();
    pen = QPen();

    for (int i = 0; i < points.length(); ++i) {
        QPointF p = points[i];
        points[i] = QPointF(p.x(), p.y());
        painter->setBrush(QBrush(QColor(mode == Mode::POINT && i == selectedIndex ? selectedColor : defaultColor)));
        painter->setPen(QColor("transparent"));
        painter->drawEllipse(p, circleRad, circleRad);
    }

    if (mode == Mode::POLYGON && selectedIndex > -1) {
        Rect rect = getRectLine(selectedIndex, penWidth);
        painter->setBrush(QBrush(QColor(selectedColor)));
        painter->setPen(QColor("transparent"));
        rect.draw([painter](const QVector<QPointF>& points) {
            painter->drawPolygon(points);
        });
    }

    painter->restore();
}

Point Polygon::getPoint(int index)
{
    return Point(points[index], circleRad);
}

void Polygon::translatePoint(QPointF v, int index)
{
    QPointF p = points[index];
    points[index] = QPointF(p.x() + v.x(), p.y() + v.y());
}

void Polygon::translateSide(QPointF v, int index)
{
    int ind1 = (index + 1) % points.length();
    int ind2 = (index) % points.length();
    QPointF p1 = points[ind1];
    QPointF p2 = points[ind2];
    points[ind1] = QPointF(p1.x() + v.x(), p1.y() + v.y());
    points[ind2] = QPointF(p2.x() + v.x(), p2.y() + v.y());
}

Rect Polygon::getRectLine(int index, float height)
{
    int ind1 = (index + 1) % points.length();
    int ind2 = (index) % points.length();
    return Rect(points[ind1], points[ind2], height);
}

int Polygon::rectLineSize()
{
    return points.length();
}
