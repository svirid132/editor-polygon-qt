#ifndef POLYGON_H
#define POLYGON_H

#include "geometrybase.h"
#include "point.h"
#include "rect.h"

class Polygon : public GeometryBase
{
public:
    enum Mode {
        POINT,
        POLYGON
    };
    Polygon(const QVector<QPointF>& points = QVector<QPointF>());
    QVector<QPointF> getPoints();
    QPointF leftBottomPoint();
    void draw(QPainter* painter, int selectedIndex = -1, Mode mode = Mode::POINT);
    Point getPoint(int index);
    void translatePoint(QPointF v, int index);
    void translateSide(QPointF v, int index);
    Rect getRectLine(int index, float height = 1);
    int rectLineSize();

private:
    float circleRad = 4;
};

#endif // POLYGON_H
