#ifndef EDITORPOLYGON_H
#define EDITORPOLYGON_H

#include <QMatrix3x3>
#include <QObject>
#include <QQuickPaintedItem>

typedef QVector<QPointF> Rect;

class EditorPolygon : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QVector<QPointF> points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(QVector<QPointF> outPoints READ outPoints NOTIFY outPointsChanged)
    Q_PROPERTY(qreal scalePoints READ scalePoints WRITE setScalePoints NOTIFY scalePointsChanged)
    Q_PROPERTY(qreal contentX READ contentX WRITE setContentX NOTIFY contentXChanged)
    Q_PROPERTY(qreal contentY READ contentY WRITE setContentY NOTIFY contentYChanged)
    Q_PROPERTY(qreal offsetDash READ offsetDash WRITE setOffsetDash NOTIFY offsetDashChanged)
    Q_PROPERTY(NOTIFY clickBack)

public:
    explicit EditorPolygon(QQuickItem *parent = nullptr);

    QVector<QPointF> points();
    void setPoints(const QVector<QPointF>& points);
    QVector<QPointF> outPoints();

    QVector<QPointF> findFrame();

    // zoom
    void setScalePoints(qreal scalePoints);
    qreal scalePoints();
    qreal contentX();
    void setContentX(qreal x);
    qreal contentY();
    void setContentY(qreal y);

    void calcZoom();

    void paint(QPainter *painter) override;

private:
    //points
    QVector<QPointF> m_points;
    QVector<QPointF> m_outPoints;

    //frame
    const float widthSidePen = 5;
    const float offsetSide = 20;
    int currentIndexSide = -1;
    bool isVisibleFrame = true;
    QVector<QPointF> m_frame;
    float cornerWidth = widthSidePen;
    float rotateWidth = cornerWidth * 2;

    //подсвечивание индикаторов, значение index
    int sideActive = -1;
    int cornerActive = -1;
    bool rotateActive = false;

    //polygon
    float circleRad = 5;
    int indexPoint = -1;

    //saved
    QPointF savedPos;
    float angleFrame;
    QVector<QPointF> saved_m_frame;
    QVector<QPointF> saved_m_points;
    float savedWidthFrame;
    float savedHeightFrame;

    // zoom
    qreal m_scalePoints = 1;
    qreal m_contentX = 0;
    qreal m_contentY = 0;

    enum StateChanged {
        NoChanged,
        Point,
        Transition,
        Scale,
        Corner,
        Rotate
    } currentState;

    QPointF scalePoint(const QPointF& p);
    QPointF unscalePoint(const QPointF& p);

    qreal m_offsetDash = 0;
    void setOffsetDash(qreal offset);
    qreal offsetDash();

    QPointF calcUnscalePoint(const QPointF& point);
    QVector<Rect> cornerFrame(const QVector<QPointF>& frame);
    QVector<Rect> sideFrame(const QVector<QPointF>& frame);
    QVector<Rect> rotateFrame(const QVector<QPointF>& frame);

signals:
    void outPointsChanged();
    void pointsChanged();
    void offsetDashChanged();
    void scalePointsChanged();
    void contentXChanged();
    void contentYChanged();
    void clickBack();

    // QQuickItem interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
};

#endif // EDITORPOLYGON_H
