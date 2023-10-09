#ifndef AMATH_H
#define AMATH_H

#include <QPointF>

// algebra math
namespace amath {
    QPointF vector(QPointF end, QPointF begin);
    float dotProduct(QPointF v1, QPointF v2);
    float calcLength(QPointF v);
    QPointF unit(QPointF v);
    QPointF normal(QPointF v);
    QPointF negative(QPointF v);
    QPointF sum(QPointF v1, QPointF v2);
    QPointF multi(QPointF v, float multi);

    class Matrix {
        public:
        Matrix(float a, float b, float c, float d, float e, float f) {
            this->a = a; this->b = b;
            this->c = c; this->d = d;
            this->e = e; this->f = f;
        }

        Matrix multi(const Matrix& m) {
            return Matrix(
                        a * m.a + c * m.b, b * m.a + d * m.b,
                        a * m.c + c * m.d, b * m.c + d * m.d,
                        a * m.e + c * m.f + e, b * m.e + d * m.f + f
                        );
        }
        QPointF multi(QPointF v) {
            return QPointF(
                        a * v.x() + c * v.y() + e,
                        b * v.x() + d * v.y() + f
                        );
        }

    private:
        float a; float b;
        float c; float d;
        float e; float f;
    };
}

#endif // AMATH_H
