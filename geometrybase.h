#ifndef GEOMETRYBASE_H
#define GEOMETRYBASE_H

#include <QPointF>
#include <QVector>

class QPainter;

class GeometryBase
{
public:
    void rotate(float angle, QPointF origin);
    void translate(QPointF v);
    void scale(float Sx, float Sy, QPointF origin, float angle = 0);
    virtual void draw(QPainter* painter);
    void draw(std::function<void(const QVector<QPointF>&)> func);
    virtual float width();
    virtual float height();
    static void zoom(GeometryBase* base, float contentX, float contentY, float scale);
    int size();
    QPointF operator[](int index);

protected:
    QVector<QPointF> points;
};

#endif // GEOMETRYBASE_H
