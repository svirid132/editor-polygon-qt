#ifndef CMATH_H
#define CMATH_H

#include <QPointF>
#include <QVector2D>

namespace CMath {
    //Находиться ли точка point внутри прямоугольника; точки p4 p1 p2 образуют угол.
    bool isContainsRect(QPointF point, QPointF p4, QPointF p1, QPointF p2);
    bool isContainsCircle(const QPointF &p, const QPointF& c, const float r);
    QPointF findUnitVector(QPointF p1, QPointF p2);
    QPointF findUnitVector(QPointF v);
    QVector2D calcNormal(QPointF p1, QPointF p2);
    float calcLength(QPointF p1, QPointF p2 = QPointF(0, 0));
    float dotProduct(QPointF p1, QPointF p2);
    QPointF rotate90(QPointF p);
    QPointF rotate90N(QPointF p);
    QPointF rotate45(QPointF p);
    QPointF rotate45N(QPointF p);
    QPointF multiPoint(float multi, QPointF p);
    QPointF calcVector(QPointF p1, QPointF p2);
    QPointF sumPoint(QPointF p1, QPointF p2);
}

#endif // CMATH_H
