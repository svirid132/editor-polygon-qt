#ifndef RECT_H
#define RECT_H

#include <QPointF>
#include <QVector>
#include "geometrybase.h"
#include <functional>

// points - Обход по часовой стерлке; не содержит заключающей точки
class Rect : public GeometryBase {
public:
    Rect(QPointF p = QPointF(), float width = 0, float height = 0); // конструкор создает от верхней левой точки прямоугольник
    Rect(QPointF end, QPointF start, float height);
    bool contains(QPointF p);
    float rotationAngle();
    QPointF center();
    float width() override;
    float height() override;
    void forEachPoint(std::function<void(QPointF)> func);
};

#endif // RECT_H
