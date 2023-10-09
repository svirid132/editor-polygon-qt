#include "editorpolygon.h"
#include "matrix.h"

#include <QDebug>
#include <QMatrix3x3>
#include <QPainter>
#include <QPolygonF>
#include <QVector>
#include <cmath>
#include <algorithm>
#include <QPropertyAnimation>
#include <QCursor>
#include <QDebug>
#include <QPainterPath>
#include <QDebug>
#include <QIcon>
#include "path.h"
#include "point.h"
#include "rect.h"
#include <cmath>
#include <functional>

EditorPolygon::EditorPolygon(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    QPropertyAnimation *anim = new QPropertyAnimation(this, "offsetDash", this);
    anim->setDuration(2000);
    anim->setStartValue(0);
    anim->setEndValue(24);
    anim->setLoopCount(-1);
    anim->start();


    verSvg = QIcon(":/resize.svg").pixmap(QSize(24, 24));
    QMatrix m;
    m.rotate(90);
    horSvg = verSvg.transformed(m);
    m = QMatrix();
    m.rotate(45);
    topCrossSvg = horSvg.transformed(m);
    m = QMatrix();
    m.rotate(-45);
    bottomCrossSvg = horSvg.transformed(m);
    rotateSvg = QIcon(":/rotate.svg").pixmap(QSize(24, 24));
}

QVector<QPointF> EditorPolygon::points()
{
    return m_points;
}

void EditorPolygon::setPoints(const QVector<QPointF>& points)
{
    if (m_points == points) {
        return;
    }
    m_points = points;
    auto ps = m_points;
    ps.removeLast();
    polygon = Polygon(ps);
    frame = findFrame();
    update();
    emit pointsChanged();
}

QVector<QPointF> EditorPolygon::outPoints()
{
    auto points = polygon.getPoints();
    if (points.size() > 0) {
        points.append(points[0]);
    }
    return points;
}

void EditorPolygon::update()
{
    Polygon zoomedPolygon = this->polygon;
    Polygon::zoom(&zoomedPolygon, contentX(), contentY(), scalePoints());
    this->zoomedPolygon = zoomedPolygon;
    updateFramePaths();
    QQuickPaintedItem::update();
}

void EditorPolygon::updateFramePaths() {
    std::array<Rect, 4> corners;
    std::array<Rect, 4> rotations;
    float halfFrameWidth = frame.width() / 2;
    float halfFrameHeight = frame.height() / 2;
    float halfBorder = frameBorder / 2;
    float unzoomedHalfBorder = halfBorder / m_scale;
    float unzoomedCornerSize = frameCornerSize / m_scale;
    float unzoomedRotateSize = frameRotateSize / m_scale;
    QPointF frameCenter = frame.center();
    for (int index = 0; index < frameCorners.size(); ++index) {
        Rect corrnerRect;
        Rect rotateRect;
        if (index == 0) {
            QPointF p = QPointF(frameCenter.x() - halfFrameWidth - unzoomedHalfBorder, frameCenter.y() - halfFrameHeight - unzoomedHalfBorder);
            corrnerRect = Rect(p, unzoomedCornerSize, unzoomedCornerSize);
            rotateRect = Rect(p, unzoomedRotateSize, unzoomedRotateSize);
        } else if (index == 1) {
            QPointF p = QPointF(frameCenter.x() + halfFrameWidth + unzoomedHalfBorder - unzoomedCornerSize, frameCenter.y() - unzoomedHalfBorder - halfFrameHeight);
            QPointF rotateP = QPointF(frameCenter.x() + halfFrameWidth + unzoomedHalfBorder - unzoomedRotateSize, frameCenter.y() - unzoomedHalfBorder - halfFrameHeight);
            corrnerRect = Rect(p, unzoomedCornerSize, unzoomedCornerSize);
            rotateRect = Rect(rotateP, unzoomedRotateSize, unzoomedRotateSize);
        } else if (index == 2) {
            QPointF p = QPointF(frameCenter.x() + halfFrameWidth + unzoomedHalfBorder - unzoomedCornerSize, frameCenter.y() + halfFrameHeight + unzoomedHalfBorder - unzoomedCornerSize);
            QPointF rotateP = QPointF(frameCenter.x() + halfFrameWidth + unzoomedHalfBorder - unzoomedRotateSize, frameCenter.y() + halfFrameHeight + unzoomedHalfBorder - unzoomedRotateSize);
            corrnerRect = Rect(p, unzoomedCornerSize, unzoomedCornerSize);
            rotateRect = Rect(rotateP, unzoomedRotateSize, unzoomedRotateSize);
        } else if (index == 3) {
            QPointF p = QPointF(frameCenter.x() - unzoomedHalfBorder - halfFrameWidth, frameCenter.y() + halfFrameHeight + unzoomedHalfBorder - unzoomedCornerSize);
            QPointF rotateP = QPointF(frameCenter.x() - unzoomedHalfBorder - halfFrameWidth, frameCenter.y() + halfFrameHeight + unzoomedHalfBorder - unzoomedRotateSize);
            corrnerRect = Rect(p, unzoomedCornerSize, unzoomedCornerSize);
            rotateRect = Rect(rotateP, unzoomedRotateSize, unzoomedRotateSize);
        }
        corrnerRect.rotate(frame.rotationAngle(), frame.center());
        rotateRect.rotate(frame.rotationAngle(), frame.center());
        corners[index] = corrnerRect;
        rotations[index] = rotateRect;
    }
    this->frameCorners = corners;
    this->frameRotates = rotations;
    std::array<Rect, 4> sides;
    float frameWidth = frame.width();
    float frameHeight = frame.height();
    for (int index = 0; index < frameSides.size(); ++index) {
        Rect rect;
        if (index == 0) {
            QPointF p = QPointF(frameCenter.x() - halfFrameWidth - unzoomedHalfBorder, frameCenter.y() - halfFrameHeight - unzoomedHalfBorder);
            rect = Rect(p, frameWidth + 2 * unzoomedHalfBorder, 2 * unzoomedHalfBorder);
        } else if (index == 1) {
            QPointF p = QPointF(frameCenter.x() + halfFrameWidth - unzoomedHalfBorder, frameCenter.y() - unzoomedHalfBorder - halfFrameHeight);
            rect = Rect(p, 2 * unzoomedHalfBorder, frameHeight + 2 * unzoomedHalfBorder);
        } else if (index == 2) {
            QPointF p = QPointF(frameCenter.x() - halfFrameWidth - unzoomedHalfBorder, frameCenter.y() + halfFrameHeight - unzoomedHalfBorder);
            rect = Rect(p, frameWidth + 2 * unzoomedHalfBorder, 2 * unzoomedHalfBorder);
        } else if (index == 3) {
            QPointF p = QPointF(frameCenter.x() - halfFrameWidth - unzoomedHalfBorder, frameCenter.y() - halfFrameHeight - unzoomedHalfBorder);
            rect = Rect(p, 2 * unzoomedHalfBorder, frameHeight + 2 * unzoomedHalfBorder);
        }
        rect.rotate(frame.rotationAngle(), frame.center());
        sides[index] = rect;
    }
    this->frameSides = sides;
}

Rect EditorPolygon::findFrame()
{
    QPointF point = polygon.leftBottomPoint();
    frame = Rect(QPointF(point.x() - polygonPadding, point.y() - polygonPadding),
                 polygon.width() + polygonPadding * 2,
                 polygon.height() + polygonPadding * 2
                 );
    return frame;
}

void EditorPolygon::setScalePoints(qreal scalePoints)
{
    if (m_scale == scalePoints) {
        return;
    }
    m_scale = scalePoints;
    update();
    emit scalePointsChanged();
}

qreal EditorPolygon::scalePoints()
{
    return m_scale;
}

qreal EditorPolygon::contentX()
{
    return m_contentX;
}

void EditorPolygon::setContentX(qreal x)
{
    if (m_contentX == x) {
        return;
    }

    m_contentX = x;
    update();
    emit contentXChanged();
}

qreal EditorPolygon::contentY()
{
    return m_contentY;
}

void EditorPolygon::setContentY(qreal y)
{
    if (m_contentY == y) {
        return;
    }

    m_contentY = y;
    update();
    emit contentYChanged();
}

void EditorPolygon::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    //Polygon
    int selectedIndex = -1;
    Polygon::Mode mode = selectedElement.type == Element::PolygonPolygon ? Polygon::POLYGON : Polygon::POINT;
    if (selectedElement.type == Element::PolygonPoint || selectedElement.type == Element::PolygonPolygon) {
        selectedIndex = selectedElement.index;
    }
    zoomedPolygon.draw(painter, selectedIndex, mode);

    if (frameOpacity() == 0.0) {
        return;
    }

    painter->setOpacity(frameOpacity());

    // frame
    Rect zoomedFrame = this->frame;
    Rect::zoom(&zoomedFrame, contentX(), contentY(), scalePoints());
    zoomedFrame.draw([this, painter](const QVector<QPointF>& points) {
        painter->save();
        QPen pen = QPen();
        pen.setColor(QColor("#20ffffff"));
        pen.setWidth(frameBorder);
        painter->setPen(pen);
        painter->drawPolygon(points);
        painter->restore();
        painter->save();
        pen = QPen();
        QVector<qreal> dashes;
        qreal space = 6;
        dashes << 2 << space;
        if (selectedElement.type == Element::Rotate) {
            pen.setColor("#FFC700");
        } else {
            pen.setColor(QColor("white"));
        }
        pen.setDashPattern(dashes);
        pen.setWidth(2);
        pen.setDashOffset(m_offsetDash);
        painter->setPen(pen);
        painter->drawPolygon(points);
        painter->restore();
    });


    // индикаторы перемещение сторон у frame
    painter->save();
    painter->setPen(QPen("transparent"));
    float halfBorder = frameBorder / 2;
    float halfZoomedFrameWidth = zoomedFrame.width() / 2;
    float halfZoomedFrameHeight = zoomedFrame.height() / 2;
    QPointF center = zoomedFrame.center();
    for (int index = 0; index < frameSides.size(); ++index) {
        QPointF origin;
        const float indWidth = index % 2 ? halfZoomedFrameHeight / 2.5 : halfZoomedFrameWidth / 2.5;
        float halfIndWidth = indWidth / 2;
        if (index == 0) {
            origin = QPointF(center.x() - halfIndWidth, center.y() - halfZoomedFrameHeight - halfBorder);
        } else if (index == 1) {
            origin = QPointF(center.x() + halfZoomedFrameWidth - halfBorder , center.y() - halfIndWidth);
        } else if (index == 2) {
            origin = QPointF(center.x() - halfIndWidth, center.y() + halfZoomedFrameHeight - halfBorder);
        } else if (index == 3) {
            origin = QPointF(center.x() - halfZoomedFrameWidth - halfBorder , center.y() - halfIndWidth);
        }
        Rect rect = index % 2 ? Rect(origin, frameBorder, indWidth) : Rect(origin, indWidth, frameBorder);
        rect.rotate(zoomedFrame.rotationAngle(), zoomedFrame.center());
        rect.draw([this, painter, index](const QVector<QPointF>& points) {
            if (selectedElement.index == index && selectedElement.type == Element::Scale) {
                painter->setBrush(QBrush("#FFC700"));
            } else {
                painter->setBrush(QBrush(QColor("#2496FF")));
            }
            painter->drawPolygon(points);
        });
    }
    painter->restore();

    // индикатор углов у frame
    painter->save();
    int index = 0;
    const float cornerIndWidth = halfZoomedFrameWidth / 20;
    float zoomedFrameAngle = zoomedFrame.rotationAngle();
    QPointF zoomedFrameCenter = zoomedFrame.center();
    QPen pen;
    pen.setWidth(frameBorder);
    QPointF p2, p1, p3;
    for (int index = 0; index < frameSides.size(); ++index) {
        if (selectedElement.type == Element::Corner && selectedElement.index == index) {
            pen.setColor("#FFC700");
        } else {
            pen.setColor("#2496FF");
        }
        painter->setPen(pen);
        if (index == 0) {
            p2 = QPointF(zoomedFrameCenter.x() - halfZoomedFrameWidth, zoomedFrameCenter.y() - halfZoomedFrameHeight);
            p1 = QPointF(p2.x(), p2.y() + cornerIndWidth);
            p3 = QPointF(p2.x() + cornerIndWidth, p2.y());
        } else if (index == 1) {
            p2 = QPointF(zoomedFrameCenter.x() + halfZoomedFrameWidth, zoomedFrameCenter.y() - halfZoomedFrameHeight);
            p1 = QPointF(p2.x(), p2.y() + cornerIndWidth);
            p3 = QPointF(p2.x() - cornerIndWidth, p2.y());
        } else if (index == 2) {
            p2 = QPointF(zoomedFrameCenter.x() + halfZoomedFrameWidth, zoomedFrameCenter.y() + halfZoomedFrameHeight);
            p1 = QPointF(p2.x(), p2.y() - cornerIndWidth);
            p3 = QPointF(p2.x() - cornerIndWidth, p2.y());
        } else if (index == 3) {
            p2 = QPointF(zoomedFrameCenter.x() - halfZoomedFrameWidth, zoomedFrameCenter.y() + halfZoomedFrameHeight);
            p1 = QPointF(p2.x(), p2.y() - cornerIndWidth);
            p3 = QPointF(p2.x() + cornerIndWidth, p2.y());
        }
        Path path(QVector<QPointF>{p1, p2, p2, p3});
        path.rotate(zoomedFrameAngle, zoomedFrameCenter);
        path.draw([painter](const QVector<QPointF>& points) {
            painter->drawLines(points.data(), 2);
        });
    }
    painter->restore();

    // Провекра правильности непрорисованых элементов

//    painter->save();
//    //    painter->setBrush(QBrush("red"));
//    painter->setPen(QPen("red"));

//    // corners
//    for (int i = 0; i < frameCorners.size(); ++i) {
//        Rect rect = frameCorners[i];
//        Rect::zoom(&rect, contentX(), contentY(), scalePoints());
//        rect.draw(painter);
//    }
//    // rotations
//    for (int i = 0; i < frameRotates.size(); ++i) {
//        Rect rect = frameRotates[i];
//        Rect::zoom(&rect, contentX(), contentY(), scalePoints());
//        rect.draw(painter);
//    }
//    // sides
//    for (int i = 0; i < frameSides.size(); ++i) {
//        Rect rect = frameSides[i];
//        Rect::zoom(&rect, contentX(), contentY(), scalePoints());
//        rect.draw(painter);
//    }
//    // rectLines
//    for (int i = 0; i < zoomedPolygon.rectLineSize(); ++i) {
//        Rect rect = zoomedPolygon.getRectLine(i, rectLineHeight);
//        rect.draw(painter);
//    }
//    painter->restore();
}

QPointF EditorPolygon::scalePoint(const QPointF &p)
{
    return QPointF(p.x() * m_scale, p.y() * m_scale);
}

QPointF EditorPolygon::unscalePoint(const QPointF &p)
{
    return QPointF(p.x() / m_scale, p.y() / m_scale);
}

void EditorPolygon::setOffsetDash(qreal offset) {
    m_offsetDash = offset;
    emit offsetDashChanged();
    QQuickPaintedItem::update();
}

qreal EditorPolygon::offsetDash()
{
    return m_offsetDash;
}

qreal EditorPolygon::frameOpacity()
{
    return m_frameOpacity;
}

void EditorPolygon::setFrameOpacity(qreal opacity)
{
    if (m_frameOpacity == opacity) {
        return;
    }
    m_frameOpacity = opacity;
    QQuickPaintedItem::update();
    emit frameOpacityChanged();
}

void EditorPolygon::visibleFrame()
{
    if (frameOpacity() == 1.0) {
        return;
    }
    QPropertyAnimation *anim = new QPropertyAnimation(this, "frameOpacity", this);
    anim->setDuration(200);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start();
}

void EditorPolygon::unvisibleFrame()
{
    if (frameOpacity() == 0.0) {
        return;
    }
    QPropertyAnimation *anim = new QPropertyAnimation(this, "frameOpacity", this);
    anim->setDuration(200);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->start();
}

QPointF EditorPolygon::unzoomPoint(const QPointF &point)
{
    return QPointF((point.x() - m_contentX) / m_scale,
                   (point.y() - m_contentY) / m_scale);
}

void EditorPolygon::mousePressEvent(QMouseEvent *event)
{
    QPointF pos = unzoomPoint(event->localPos());
    savedPos = pos;
    const int frameSize = 4; // стороны, углы
    savedPolygon = polygon;
    savedFrame = frame;
    update();
    for (int index = 0; index < frameSize; ++index) {
        if (frameCorners[index].contains(pos)) {
            selectedElement.type = Element::Corner;
            selectedElement.index = index;
            return;
        } else if (frameRotates[index].contains(pos)) {
            selectedElement.type = Element::Rotate;
            selectedElement.index = index;
            return;
        }
    }
    for (int index = 0; index < frameSize; ++index) {
        if (frameSides[index].contains(pos)) {
            selectedElement.type = Element::Scale;
            selectedElement.index = index;
            return;
        }
    }
    if (m_mode == Mode::POINT) {
        for (int index = 0; index < polygon.size(); ++index) {
            Point p = polygon.getPoint(index);
            p.setRadius(p.radius() / scalePoints());
            if (p.contains(pos)) {
                selectedElement.type = Element::PolygonPoint;
                selectedElement.index = index;
                unvisibleFrame();
                return;
            }
        }
    } else if (m_mode == Mode::POLYGON) {
        QPointF pos = event->localPos();
        for (int index = 0; index < polygon.rectLineSize(); ++index) {
            Rect rect = zoomedPolygon.getRectLine(index, rectLineHeight);
            if (rect.contains(pos)) {
                selectedElement.type = Element::PolygonPolygon;
                selectedElement.index = index;
                unvisibleFrame();
                return;
            }
        }
    }
    if (frame.contains(pos)) {
        selectedElement.type = Element::Transition;
        selectedElement.index = -1;
        return;
    }
}

void EditorPolygon::mouseMoveEvent(QMouseEvent *event)
{
    if (selectedElement.type == Element::NoType) {
        return;
    }
    QPointF pos = unzoomPoint(event->localPos());
    Polygon newPolygon = savedPolygon;
    Rect newFrame = savedFrame;
    auto updateFrame = [&]() {
        frame = newFrame;
        polygon = newPolygon;
        update();
    };
    if (selectedElement.type == Element::Transition) {
        QPointF v = amath::vector(pos, savedPos);
        newFrame.translate(v);
        newPolygon.translate(v);
        updateFrame();
    } else if (selectedElement.type == Element::Scale || selectedElement.type == Element::Corner) {
        float minSize = 10;
        int ind = selectedElement.index;
        QPointF trans = amath::vector(pos, savedPos);
        QPointF v1 = amath::vector(savedFrame[ind], savedFrame[ind + 1]);
        QPointF normUnit1 = amath::normal(amath::unit(v1));
        QPointF v2 = amath::vector(savedFrame[ind + 3], savedFrame[ind + 4]);
        QPointF normUnit2 = amath::normal(amath::unit(v2));
        if (selectedElement.type == Element::Scale) {
            if (ind % 2 == 1) {
                float dx = amath::dotProduct(normUnit1, trans);
                float Sx = 1 + dx / savedFrame.width();
                if (savedFrame.width() * Sx < minSize) {
                    return;
                }
                newFrame.scale(Sx, 1, savedFrame[ind + 2], savedFrame.rotationAngle());
                newPolygon.scale(Sx, 1, savedFrame[ind + 2], savedFrame.rotationAngle());
            } else {
                float dy = amath::dotProduct(normUnit1, trans);
                float Sy = 1 + dy / savedFrame.height();
                if (savedFrame.height() * Sy < minSize) {
                    return;
                }
                newFrame.scale(1, Sy, savedFrame[ind + 2], savedFrame.rotationAngle());
                newPolygon.scale(1, Sy, savedFrame[ind + 2], savedFrame.rotationAngle());
            }
        } else if (selectedElement.type == Element::Corner) {
            float dx = amath::dotProduct(ind % 2 == 0 ? normUnit2: normUnit1 , trans);
            float dy = amath::dotProduct(ind % 2 == 0 ? normUnit1 : normUnit2, trans);
            float Sx = 1 + dx / savedFrame.width();
            float Sy = 1 + dy / savedFrame.height();
            if (savedFrame.width() * Sx < minSize) {
                return;
            }
            if (savedFrame.height() * Sy < minSize) {
                return;
            }
            newFrame.scale(Sx, Sy, savedFrame[ind + 2], savedFrame.rotationAngle());
            newPolygon.scale(Sx, Sy, savedFrame[ind + 2], savedFrame.rotationAngle());
        }
        updateFrame();
    } else if (selectedElement.type == Element::Rotate) {
        QPointF center = savedFrame.center();
        QPointF beginVector = amath::vector(savedPos, center);
        QPointF ordinatVector = amath::normal(beginVector);
        QPointF endVector = amath::vector(pos, center);
        float cos = amath::dotProduct(beginVector, endVector) / (amath::calcLength(beginVector) * amath::calcLength(endVector));
        float arccos = std::acos(cos);
        float angle = amath::dotProduct(ordinatVector, endVector) > 0 ? arccos : - arccos;
        newFrame.rotate(angle, savedFrame.center());
        newPolygon.rotate(angle, savedFrame.center());
        updateFrame();
    } else if (selectedElement.type == Element::PolygonPolygon) {
        QPointF v = amath::vector(pos, savedPos);
        newPolygon.translateSide(v, selectedElement.index);
        updateFrame();
    } else if (selectedElement.type == Element::PolygonPoint) {
        QPointF v = amath::vector(pos, savedPos);
        newPolygon.translatePoint(v, selectedElement.index);
        updateFrame();
    }
}

void EditorPolygon::mouseReleaseEvent(QMouseEvent *event)
{
    if (selectedElement.type == Element::PolygonPoint || selectedElement.type == Element::PolygonPolygon) {
        frame = findFrame();
        update();
        visibleFrame();
    }
    selectedElement.type = Element::NoType;
    selectedElement.index = -1;
    update();
}


void EditorPolygon::hoverMoveEvent(QHoverEvent *event)
{
    float angle = frame.rotationAngle();
    QPointF pos = event->posF();
    pos = unzoomPoint(pos);

    const float angle45 = M_PI / 4;
    const float angle90 = M_PI / 2;

    std::function<bool(float)> cursorCb = [this](float angle) {

        const float angle30 = M_PI / 6;
        const float angle60 = M_PI / 3;
        const float angle90 = M_PI / 2;
        const float angle120 = angle90 + angle30;
        const float angle150 = angle90 + angle60;
        const float angle180 = M_PI;

        angle = angle < 0 ? M_PI + angle : angle;
        while(angle > M_PI) {
            angle -= M_PI;
        }

        if (0 <= angle && angle < angle30) {
            setCursor(verSvg);
            return true;
        } else if (0 <= angle && angle < angle60) {
            setCursor(bottomCrossSvg);
            return true;
        } else if (angle60 <= angle && angle < angle120) {
            setCursor(horSvg);
            return true;
        } else if (angle120 <= angle && angle < angle150) {
            setCursor(topCrossSvg);
            return true;
        } else if (angle150 <= angle && angle <= angle180) {
            setCursor(verSvg);
            return true;
        }
        return false;
    };

    int rectSize = frameSides.size();
    for (int i = 0; i < rectSize; ++i) {
        if (frameCorners[i].contains(pos)) {
            float handleAngle = (angle - angle45 + angle90 * (float)(i));
            if (cursorCb(handleAngle)) {
                return;
            }
        } else if (frameRotates[i].contains(pos)) {
            setCursor(rotateSvg);
            return;
        }
    }
    for (int i = 0; i < rectSize; ++i) {
        if (frameSides[i].contains(pos)) {
            float handleAngle = (angle + angle90 * (float)(i));
            if (cursorCb(handleAngle)) {
                return;
            }
        }
    }

    setCursor(QCursor(Qt::ArrowCursor));
}
