#ifndef EDITORPOLYGON_H
#define EDITORPOLYGON_H

#include <QMatrix3x3>
#include <QObject>
#include <QPainter>
#include <QQuickPaintedItem>
#include "amath.h"
#include "polygon.h"
#include "rect.h"

class Rect;

class EditorPolygon : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QVector<QPointF> points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QVector<QPointF> outPoints READ outPoints NOTIFY outPointsChanged)
    Q_PROPERTY(qreal scalePoints READ scalePoints WRITE setScalePoints NOTIFY scalePointsChanged)
    Q_PROPERTY(qreal contentX READ contentX WRITE setContentX NOTIFY contentXChanged)
    Q_PROPERTY(qreal contentY READ contentY WRITE setContentY NOTIFY contentYChanged)
    Q_PROPERTY(qreal frameOpacity READ frameOpacity WRITE setFrameOpacity NOTIFY frameOpacityChanged)
    Q_PROPERTY(qreal offsetDash READ offsetDash WRITE setOffsetDash NOTIFY offsetDashChanged)
    Q_PROPERTY(NOTIFY clickBack)

public:
    explicit EditorPolygon(QQuickItem *parent = nullptr);

    enum Mode {
        POINT,
        POLYGON
    };
    Q_ENUM(Mode)

    Mode mode() {
        return m_mode;
    }
    void setMode(Mode mode) {
        if (m_mode == mode) {
            return;
        }

        m_mode = mode;
        emit modeChanged();
    }
    Mode m_mode = Mode::POINT;

    QVector<QPointF> points();
    void setPoints(const QVector<QPointF>& points);
    QVector<QPointF> outPoints();
    void update();

    Rect findFrame();

    // zoom
    void setScalePoints(qreal scalePoints);
    qreal scalePoints();
    qreal contentX();
    void setContentX(qreal x);
    qreal contentY();
    void setContentY(qreal y);

    void paint(QPainter *painter) override;

private:
    QVector<QPointF> m_points; // входные точки полигона
    Polygon polygon; // изменыне точки полигона
    Polygon savedPolygon;

    QPointF savedPos = QPointF(-1, -1);

    // параметры frame
    Rect frame;
    Rect savedFrame;
    const float frameBorder = 5;
    const float polygonPadding = 10;
    float frameCornerSize = frameBorder * 2; // corner - угол, который увеличивает рамку по Sx, Sy
    float frameRotateSize = frameCornerSize * 1.5; // размер прямоугольника по повороту рамки
    bool frameVisible = true;

    // пунктирная линия frame
    qreal m_offsetDash = 0;
    void setOffsetDash(qreal offset);
    qreal offsetDash();

    // opacity исчезновение
    qreal m_frameOpacity = 1.0;
    qreal frameOpacity();
    void setFrameOpacity(qreal opacity);
    void visibleFrame();
    void unvisibleFrame();

    enum Element {
        NoType,
        PolygonPoint, // перемещение точки - mode.POINT
        PolygonPolygon, // перемещение стороны - mode.Polygon
        Transition, // заполненное простравнство frame
        Scale, // сторона frame
        Corner,
        Rotate
    };
    struct SelectedElement {
        Element type;
        int index = -1;
    } selectedElement;

    // polygon - настройка полигона
    Polygon zoomedPolygon;
    float rectLineHeight = 2;
    float circleRad = 5;

    // zoom
    qreal m_scale = 1;
    qreal m_contentX = 0;
    qreal m_contentY = 0;
    QPointF scalePoint(const QPointF& p);
    QPointF unscalePoint(const QPointF& p);
    QPointF unzoomPoint(const QPointF& point);

    std::array<Rect, 4> frameCorners;
    std::array<Rect, 4> frameSides;
    std::array<Rect, 4> frameRotates;
    void updateFramePaths();

    QPixmap verSvg;
    QPixmap horSvg;
    QPixmap topCrossSvg;
    QPixmap bottomCrossSvg;
    QPixmap rotateSvg;

signals:
    void outPointsChanged();
    void pointsChanged();
    void offsetDashChanged();
    void scalePointsChanged();
    void contentXChanged();
    void contentYChanged();
    void clickBack();
    void frameOpacityChanged();
    void modeChanged();

    // QQuickItem interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
};

#endif // EDITORPOLYGON_H
