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

#include "cmath.h"
using namespace CMath;

EditorPolygon::EditorPolygon(QQuickItem *parent)
    : QQuickPaintedItem{parent}
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    QPropertyAnimation *anim = new QPropertyAnimation(this, "offsetDash", this);
    anim->setDuration(2000);
    anim->setStartValue(0);
    anim->setEndValue(24);
    anim->setLoopCount(-1);
    anim->start();
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
    qDebug() << "points:" <<points;
    m_points = points;
    m_outPoints = points;
    m_frame = findFrame();

    update();
    emit pointsChanged();
}

QVector<QPointF> EditorPolygon::outPoints()
{
    return m_outPoints;
}

QVector<QPointF> EditorPolygon::findFrame()
{
    if (m_outPoints.size() == 0) {
        return QVector<QPointF>();
    }
    QPointF bottomLeft = m_outPoints[0]; QPointF topRight = m_outPoints[0];
    std::for_each(m_outPoints.begin(), m_outPoints.end(), [&](const QPointF& point) {
        if (point.x() < bottomLeft.x()) {
            bottomLeft.setX(point.x());
        }
        if (point.y() < bottomLeft.y()) {
            bottomLeft.setY(point.y());
        }
        if (point.x() > topRight.x()) {
            topRight.setX(point.x());
        }
        if (point.y() > topRight.y()) {
            topRight.setY(point.y());
        }
    });
    QVector<QPointF> frame;
    const float offset = offsetSide + widthSidePen / 2;
    frame << QPointF(bottomLeft.x() - offset, bottomLeft.y() - offset)
          << QPointF(topRight.x() + offset, bottomLeft.y() - offset)
          << QPointF(topRight.x() + offset, topRight.y() + offset)
          << QPointF(bottomLeft.x() - offset, topRight.y() + offset)
          << QPointF(bottomLeft.x() - offset, bottomLeft.y() - offset);

    return frame;
}

void EditorPolygon::setScalePoints(qreal scalePoints)
{
    if (m_scalePoints == scalePoints) {
        return;
    }
    m_scalePoints = scalePoints;
    update();
    emit scalePointsChanged();
}

qreal EditorPolygon::scalePoints()
{
    return m_scalePoints;
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

void EditorPolygon::calcZoom()
{
//    QVector<QPointF> newPoints = QVector<QPointF>(m_outPoints.length());
//    qDebug() << m_contentX;
//    for (int i = 0; i < m_outPoints.length(); ++i) {
//        newPoints[i] = QPointF(m_contentX + m_outPoints[i].x() * m_scalePoints,
//                               m_contentY + m_outPoints[i].y() * m_scalePoints);
//    }
//    m_outPoints = newPoints;
//    m_frame = findFrame();

}

void EditorPolygon::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    QVector<QPointF> scalePolygon = QVector<QPointF>(m_outPoints.length());
    for (int i = 0; i < m_outPoints.length(); ++i) {
        scalePolygon[i] = QPointF(m_contentX + m_outPoints[i].x() * m_scalePoints,
                               m_contentY + m_outPoints[i].y() * m_scalePoints);
    }
    QVector<QPointF> scaleFrame = QVector<QPointF>(m_frame.length());
    for (int i = 0; i < scaleFrame.length(); ++i) {
        scaleFrame[i] = QPointF(m_contentX + m_frame[i].x() * m_scalePoints,
                               m_contentY + m_frame[i].y() * m_scalePoints);
    }

    //Polygon
    QPolygonF polygon(scalePolygon);
    painter->save();
    QPen pen;
    pen.setColor(QColor("white"));
    painter->setPen(pen);
    painter->drawPolygon(polygon);
    painter->restore();

    //Polygon circle
    painter->save();
    pen = QPen();
    for (int i = 0; i < scalePolygon.length(); ++i) {
        QPointF p = scalePolygon[i];
        polygon[i] = QPointF(p.x(), p.y());
        painter->setBrush(QBrush(QColor("white")));
        painter->setPen(QColor("transparent"));
        painter->drawEllipse(p, circleRad, circleRad);
    }
    painter->restore();
    if (m_frame.size() != 5 || !isVisibleFrame) {
        return;
    }

    //side frame
    painter->save();
    pen = QPen();
    pen.setColor(QColor("#20ffffff"));
    pen.setWidth(widthSidePen);
    painter->setPen(pen);
    painter->drawPolygon(scaleFrame);
    painter->restore();

    //dash frame
    painter->save();
    pen = QPen();
    QVector<qreal> dashes;
    qreal space = 6;
    dashes << 2 << space;
    if (rotateActive) {
        pen.setColor("#FFC700");
    } else {
        pen.setColor(QColor("white"));
    }
    pen.setDashPattern(dashes);
    pen.setWidth(2);
    pen.setDashOffset(m_offsetDash);
    painter->setPen(pen);
    painter->drawPolygon(scaleFrame);
    painter->restore();

    //indicator frame side
    painter->save();
    const float widthIndSide = 40 * m_scalePoints;
    const float widthFrame = calcLength(scaleFrame[0], scaleFrame[1]);
    const float heightFrame = calcLength(scaleFrame[1], scaleFrame[2]);
    float offsetWidth = widthFrame / 2 - widthIndSide / 2;
    float offsetHeight = heightFrame / 2 - widthIndSide / 2;
    pen = QPen();
    pen.setWidth(widthSidePen + 2);
    painter->setPen(pen);
    for(int i = 0; i < scaleFrame.length() - 1; ++i) {
        QPointF vec = findUnitVector(scaleFrame[i], scaleFrame[i + 1]);
        if (sideActive == i) {
            pen.setColor("#FFC700");
        } else {
            pen.setColor(QColor("#2496FF"));
        }
        painter->setPen(pen);
        if (i % 2) {
            QPointF beginP = QPointF(scaleFrame[i].x() + vec.x() * offsetHeight, scaleFrame[i].y() + vec.y()* offsetHeight);
            QPointF endP = QPointF(beginP.x() + vec.x() * widthIndSide, beginP.y() + vec.y() * widthIndSide);
            painter->drawLine(beginP, endP);
        } else {
            QPointF beginP = QPointF(scaleFrame[i].x() + vec.x() * offsetWidth, scaleFrame[i].y() + vec.y()* offsetWidth);
            QPointF endP = QPointF(beginP.x() + vec.x() * widthIndSide, beginP.y() + vec.y() * widthIndSide);
            painter->drawLine(beginP, endP);
        }
    }
    painter->restore();

    //indicator frame corner
    painter->save();
    const float widthIndCorner = 7 * m_scalePoints;
    pen = QPen();
    pen.setWidth(widthSidePen + 2);
    for(int i = 0; i < scaleFrame.length() - 1; ++i) {
        if (cornerActive == i) {
            pen.setColor("#FFC700");
        } else {
            pen.setColor(QColor("#2496FF"));
        }
        painter->setPen(pen);
        painter->setPen(pen);
        QPointF vec = findUnitVector(scaleFrame[i], scaleFrame[i + 1]);
        QPointF endP_1 = QPointF(scaleFrame[i].x() + vec.x() * widthIndCorner, scaleFrame[i].y() + vec.y() * widthIndCorner);
        QPointF endP_2 = QPointF(scaleFrame[i].x() - vec.y() * widthIndCorner, scaleFrame[i].y() + vec.x() * widthIndCorner); // перевенули на 90 градусов
        painter->drawLine(scaleFrame[i], endP_1);
        painter->drawLine(scaleFrame[i], endP_2);
    }
    painter->restore();

//    //corners
//    auto corners = cornerFrame(scaleFrame);
//    for (int i = 0; i < corners.length(); ++i) {
//        for (int a = 0; a < 4; ++a) {
//            painter->drawLine(corners[i][a], corners[i][(a+1)%4]);
//        }
//    }
//    //rotate
//    auto rotate = rotateFrame(scaleFrame);
//    for (int i = 0; i < corners.length(); ++i) {
//        for (int a = 0; a < 4; ++a) {
//            painter->drawLine(rotate[i][a], rotate[i][(a+1)%4]);
//        }
//    }
//    //sides
//    auto sides = sideFrame(scaleFrame);
//    for (int i = 0; i < sides.length(); ++i) {
//        for (int a = 0; a < 4; ++a) {
//            painter->drawLine(sides[i][a], sides[i][(a+1)%4]);
//        }
//    }
}

QPointF EditorPolygon::scalePoint(const QPointF &p)
{
    return QPointF(p.x() * m_scalePoints, p.y() * m_scalePoints);
}

QPointF EditorPolygon::unscalePoint(const QPointF &p)
{
    return QPointF(p.x() / m_scalePoints, p.y() / m_scalePoints);
}

void EditorPolygon::setOffsetDash(qreal offset) {
    m_offsetDash = offset;
    emit offsetDashChanged();
    update();
}

qreal EditorPolygon::offsetDash()
{
    return m_offsetDash;
}

QPointF EditorPolygon::calcUnscalePoint(const QPointF &point)
{
    return QPointF((point.x() - m_contentX) / m_scalePoints,
                                   (point.y() - m_contentY) / m_scalePoints);
}

QVector<Rect> EditorPolygon::cornerFrame(const QVector<QPointF>& frame)
{
    float halfWidthSide = widthSidePen / 2;
    const float d = calcLength(QPointF(halfWidthSide, halfWidthSide));
    const float rectWidth = cornerWidth;
    QVector<Rect> corners;
    for (int i = 0; i < 4; ++i) {
        // стрелка на 10.5 часах
        QPointF v = calcVector(frame[(i + 1) % 4], frame[i]);
        v = rotate45(v);
        v = findUnitVector(v);
        QPointF originV = multiPoint(d, v);
        QPointF originP = sumPoint(frame[i], originV);
        //Стрелка на 3 часах
        v = rotate45(v); v = rotate90(v);
        QPointF rectV = multiPoint(rectWidth, v);
        Rect rect = Rect(4);
        rect[0] = originP;
        QPointF p = rect[0];
        // Стрелка на 3, 6, 9 часах
        for (int i = 1; i < 4; ++i) {
            p = sumPoint(p, rectV);
            rect[i] = p;
            rectV = rotate90(rectV);
        }
        corners.push_back(rect);
    }

    return corners;
}

QVector<Rect> EditorPolygon::sideFrame(const QVector<QPointF>& frame)
{
    float halfWidthSide = widthSidePen / 2;
    QVector<Rect> sides;
    for (int i = 0; i < 4; ++i) {
        QPointF v = calcVector(frame[i], frame[(i + 1) % 4]);
        v = rotate90(v);
        v = findUnitVector(v);
        v = multiPoint(halfWidthSide, v);
        QPointF p1 = sumPoint(frame[i], v);
        QPointF p2 = sumPoint(frame[(i + 1) % 4], v);
        v = rotate90N(v);
        v = rotate90N(v);
        QPointF p3 = sumPoint(frame[(i + 1) % 4], v);
        QPointF p4 = sumPoint(frame[i], v);
        Rect rect = {p1, p2, p3, p4};
        sides.append(rect);
    }

    return sides;
}

QVector<Rect> EditorPolygon::rotateFrame(const QVector<QPointF>& frame)
{
    float halfWidthSide = widthSidePen / 2;
    const float d = calcLength(QPointF(halfWidthSide, halfWidthSide));
    const float rectWidth = rotateWidth;
    QVector<Rect> corners;
    for (int i = 0; i < 4; ++i) {
        // стрелка на 10.5 часах
        QPointF v = calcVector(frame[i], frame[(i + 1) % 4]);
        v = rotate45(v);
        v = findUnitVector(v);
        QPointF originV = multiPoint(d, v);
        QPointF originP = sumPoint(frame[i], originV);
        //Стрелка на 3 часах
        v = rotate45(v); v = rotate90(v);
        QPointF rectV = multiPoint(rectWidth, v);
        Rect rect = Rect(4);
        rect[0] = originP;
        QPointF p = rect[0];
        // Стрелка на 3, 6, 9 часах
        for (int i = 1; i < 4; ++i) {
            p = sumPoint(p, rectV);
            rect[i] = p;
            rectV = rotate90(rectV);
        }
        corners.push_back(rect);
    }

    return corners;
}



void EditorPolygon::mousePressEvent(QMouseEvent *event)
{
    if (m_frame.size() != 5) {
        return;
    }
    savedPos = calcUnscalePoint(event->localPos());
    const QPointF scalePos = event->localPos();
    //polyon
    for (int i = 0; i < m_outPoints.length(); ++i) {
        if (isContainsCircle(savedPos, m_outPoints[i], circleRad)) {
            currentState = StateChanged::Point;
            saved_m_points = m_outPoints;
            indexPoint = i;
            isVisibleFrame = false;
            update();
            return;
        }
    }
    QVector<QPointF> scaleFrame = QVector<QPointF>(m_frame.length());
    for (int i = 0; i < scaleFrame.length(); ++i) {
        scaleFrame[i] = QPointF(m_contentX + m_frame[i].x() * m_scalePoints,
                               m_contentY + m_frame[i].y() * m_scalePoints);
    }
    auto corners = cornerFrame(scaleFrame);
    auto rotates = rotateFrame(scaleFrame);
    auto sides = sideFrame(m_frame);
    for (int i = 0; i < 4; ++i) {
        // corner
        if (isContainsRect(scalePos, corners[i][3], corners[i][0], corners[i][1])) {
            saved_m_frame = m_frame;
            saved_m_points = m_outPoints;
            currentState = Rotate;
            currentIndexSide = i;
            saved_m_frame = m_frame;
            saved_m_points = m_outPoints;
            QPointF normal = QPointF(0, 1);
            QPointF vecSide(m_frame[1].x() - m_frame[0].x(), m_frame[1].y() - m_frame[0].y());
            float dot = dotProduct(vecSide, normal);
            savedWidthFrame = calcLength(m_frame[0], m_frame[1]);
            savedHeightFrame = calcLength(m_frame[1], m_frame[2]);
            float angle = std::acos(float(vecSide.x()) / savedWidthFrame);
            angleFrame = dot > 0 ? angle : -angle;
            currentState = Corner;
            cornerActive = i;
            return;
        }
        // rotate
        if (isContainsRect(scalePos, rotates[i][3], rotates[i][0], rotates[i][1])) {
            saved_m_frame = m_frame;
            saved_m_points = m_outPoints;
            currentState = Rotate;
            rotateActive = true;
            return;
        }
    }
    for (int i = 0; i < 4; ++i) {
        // side
        if(isContainsRect(savedPos, sides[i][3], sides[i][0], sides[i][1]))
        {
            currentIndexSide = i;
            saved_m_frame = m_frame;
            saved_m_points = m_outPoints;
            QPointF normal = QPointF(0, 1);
            QPointF vecSide(m_frame[1].x() - m_frame[0].x(), m_frame[1].y() - m_frame[0].y());
            float dot = dotProduct(vecSide, normal);
            savedWidthFrame = calcLength(m_frame[0], m_frame[1]);
            savedHeightFrame = calcLength(m_frame[1], m_frame[2]);
            float angle = std::acos(float(vecSide.x()) / savedWidthFrame);
            angleFrame = dot > 0 ? angle : -angle;
            currentState = Scale;
            sideActive = i;
            return;
        }
    }
    // fill
    if(isContainsRect(savedPos, m_frame[3], m_frame[0], m_frame[1])) {
        currentState = StateChanged::Transition;
        saved_m_frame = m_frame;
        saved_m_points = m_outPoints;
        return;
    }
    emit clickBack();
}

void EditorPolygon::mouseMoveEvent(QMouseEvent *event)
{
    QPointF localPos = calcUnscalePoint(event->localPos());
    if (currentState == StateChanged::Point) {
        const QPointF vec = QPointF(localPos.x() - savedPos.x(), localPos.y() - savedPos.y());
        if (indexPoint == 0 || indexPoint == (saved_m_points.length() - 1)) {
            m_outPoints[0] = QPointF(saved_m_points[0].x() + vec.x(), saved_m_points[0].y() + vec.y());
            m_outPoints[saved_m_points.length() - 1] = QPointF(saved_m_points[saved_m_points.length() - 1].x() + vec.x(), saved_m_points[saved_m_points.length() - 1].y() + vec.y());
            update();
            return;
        }
        m_outPoints[indexPoint] = QPointF(saved_m_points[indexPoint].x() + vec.x(), saved_m_points[indexPoint].y() + vec.y());
        update();
        return;
    } else if (currentState == StateChanged::Transition) {
        QPointF transition = QPointF(localPos.x() - savedPos.x(), localPos.y() - savedPos.y());
        QVector<QPointF> new_m_frame(saved_m_frame.length());
        Matrix matrix(1, 0, 0, 1, transition.x(), transition.y());
        for (int i = 0; i < new_m_frame.length(); ++i) {
            QPointF p = saved_m_frame[i];
            new_m_frame[i] = matrix.multiVector(p);
        }
        QVector<QPointF> new_m_points(saved_m_points);
        for (int i = 0; i < saved_m_points.length(); ++i) {
            QPointF p = saved_m_points[i];
            new_m_points[i] = matrix.multiVector(p);
        }
        m_frame = new_m_frame;
        m_outPoints = new_m_points;
        update();
        return;
    } else if (currentState == StateChanged::Rotate) {
        QPointF center = QPointF(
                    (saved_m_frame[0].x() + saved_m_frame[2].x()) / 2,
                (saved_m_frame[0].y() + saved_m_frame[2].y()) / 2
                );
        QPointF beginVector = calcVector(center, savedPos);
        QPointF ordinatVector = rotate90(beginVector);
        QPointF endVector = calcVector(center, localPos);
        float cos = dotProduct(beginVector, endVector) / (calcLength(beginVector) * calcLength(endVector));
        float arccos = std::acos(cos);
        float angle = dotProduct(ordinatVector, endVector) > 0 ? arccos : - arccos;
        float a = std::cos(angle);
        float b = std::sin(angle);
        float c = -std::sin(angle);
        float d = std::cos(angle);
        float e = -center.x() * a - center.y() * c + center.x();
        float f = -center.x() * b - center.y() * d + center.y();
        QVector<QPointF> new_m_frame(saved_m_frame.length());
        Matrix matrix(a, b, c, d, e, f);
        for (int i = 0; i < new_m_frame.length(); ++i) {
            QPointF p = saved_m_frame[i];
            new_m_frame[i] = matrix.multiVector(p);
        }
        QVector<QPointF> new_m_points(saved_m_points);
        for (int i = 0; i < saved_m_points.length(); ++i) {
            QPointF p = saved_m_points[i];
            new_m_points[i] = matrix.multiVector(p);
        }
        m_frame = new_m_frame;
        m_outPoints = new_m_points;
    } else if (currentState == StateChanged::Corner) {
        const QPointF vec = QPointF(localPos.x() - savedPos.x(), localPos.y() - savedPos.y());
        const int indexAngle = (currentIndexSide + 2 ) % 4;
        const QPointF t = saved_m_frame[indexAngle];
        QPointF p1 = saved_m_frame[currentIndexSide];
        QPointF p2 = saved_m_frame[(currentIndexSide + 1) % 4];
        QPointF p4 = saved_m_frame[(currentIndexSide + 3) % 4];
        QVector2D normalP2 = calcNormal(p1, p2);
        QVector2D normalP4 = calcNormal(p4, p1);
        normalP2.normalize();
        normalP4.normalize();
        float multiX = 1; float multiY = 1;
        if (currentIndexSide % 2) {
            multiX = dotProduct(vec, normalP2.toPointF());
            multiY = dotProduct(vec, normalP4.toPointF());
        } else {
            multiX = dotProduct(vec, normalP4.toPointF());
            multiY = dotProduct(vec, normalP2.toPointF());
        }
        const float newWidth = savedWidthFrame + multiX;
        multiX = newWidth / savedWidthFrame;
        const float newHeight = savedHeightFrame + multiY;
        multiY = newHeight / savedHeightFrame;
        if (newHeight < 10 || newWidth < 10) return;
        float sin = std::sin(angleFrame);
        float cos = std::cos(angleFrame);
        const float a = multiX * cos * cos + multiY * sin * sin;
        const float b = multiX * sin * cos - multiY * cos * sin;
        const float c = multiX * cos * sin - multiY * sin * cos;
        const float d = multiX * sin * sin + multiY * cos * cos;
        const float e = -t.x() * a - t.y() * c + t.x();
        const float f = -t.x() * b - t.y() * d + t.y();
        Matrix matrix(a, b, c, d, e, f);
        QVector<QPointF> new_m_frame(saved_m_frame.length());
        for (int i = 0; i < new_m_frame.length(); ++i) {
            QPointF p = saved_m_frame[i];
            new_m_frame[i] = matrix.multiVector(p);
        }
        QVector<QPointF> new_m_points(saved_m_points);
        for (int i = 0; i < saved_m_points.length(); ++i) {
            QPointF p = saved_m_points[i];
            new_m_points[i] = matrix.multiVector(p);
        }
        m_frame = new_m_frame;
        m_outPoints = new_m_points;
        update();
        return;
    } else if (currentState == Scale) {
        const QPointF vec = QPointF(localPos.x() - savedPos.x(), localPos.y() - savedPos.y());
        const int indexAngle = (currentIndexSide + 3 ) % 4;
        const QPointF t = saved_m_frame[indexAngle];
        QPointF p1 = saved_m_frame[currentIndexSide];
        QPointF p2 = saved_m_frame[(currentIndexSide + 1) % 4];
        QVector2D normal = calcNormal(p1, p2);
        normal.normalize();
        float multi = dotProduct(vec, normal.toPointF());
        float multiX = 1; float multiY = 1;
        if (currentIndexSide % 2 == 1) {
            const float newWidth = savedWidthFrame + multi;
            if (newWidth < 10) return;
            multiX = newWidth / savedWidthFrame;
        } else {
            const float newHeight = savedHeightFrame + multi;
            if (newHeight < 10) return;
            multiY = newHeight / savedHeightFrame;
        }
        float sin = std::sin(angleFrame);
        float cos = std::cos(angleFrame);
        const float a = multiX * cos * cos + multiY * sin * sin;
        const float b = multiX * sin * cos - multiY * cos * sin;
        const float c = multiX * cos * sin - multiY * sin * cos;
        const float d = multiX * sin * sin + multiY * cos * cos;
        const float e = -t.x() * a - t.y() * c + t.x();
        const float f = -t.x() * b - t.y() * d + t.y();
        Matrix matrix(a, b, c, d, e, f);
        QVector<QPointF> new_m_frame(saved_m_frame.length());
        for (int i = 0; i < new_m_frame.length(); ++i) {
            QPointF p = saved_m_frame[i];
            new_m_frame[i] = matrix.multiVector(p);
        }
        QVector<QPointF> new_m_points(saved_m_points);
        for (int i = 0; i < saved_m_points.length(); ++i) {
            QPointF p = saved_m_points[i];
            new_m_points[i] = matrix.multiVector(p);
        }
        m_frame = new_m_frame;
        m_outPoints = new_m_points;
        update();
    }
}

void EditorPolygon::mouseReleaseEvent(QMouseEvent *event)
{
    currentIndexSide = -1;
    if (currentState == StateChanged::Point) {
        isVisibleFrame = true;
        m_frame = findFrame();
        update();
    }
    currentState = StateChanged::NoChanged;
    sideActive = -1;
    cornerActive = -1;
    rotateActive = false;
}


void EditorPolygon::hoverMoveEvent(QHoverEvent *event)
{
    if (m_frame.size() != 5) {
        return;
    }
    const QPointF savedPos = calcUnscalePoint(event->posF());
    const QPointF scalePos = event->posF();
    //polyon
    for (int i = 0; i < m_outPoints.length(); ++i) {
        if (isContainsCircle(savedPos, m_outPoints[i], circleRad)) {
            setCursor(QCursor(Qt::CrossCursor));
            return;
        }
    }
    QVector<QPointF> scaleFrame = QVector<QPointF>(m_frame.length());
    for (int i = 0; i < scaleFrame.length(); ++i) {
        scaleFrame[i] = QPointF(m_contentX + m_frame[i].x() * m_scalePoints,
                               m_contentY + m_frame[i].y() * m_scalePoints);
    }
    auto corners = cornerFrame(scaleFrame);
    auto rotates = rotateFrame(scaleFrame);
    auto sides = sideFrame(m_frame);
    for (int i = 0; i < 4; ++i) {
        // corner
        if (isContainsRect(scalePos, corners[i][3], corners[i][0], corners[i][1])) {
            setCursor(QCursor(Qt::SizeBDiagCursor));
            return;
        }
        // rotate
        if (isContainsRect(scalePos, rotates[i][3], rotates[i][0], rotates[i][1])) {
            setCursor(QCursor(Qt::SplitVCursor));
            return;
        }
    }
    for (int i = 0; i < 4; ++i) {
        // side
        if(isContainsRect(savedPos, sides[i][3], sides[i][0], sides[i][1]))
        {
            setCursor(QCursor(Qt::SizeVerCursor));
            return;
        }
    }
    // fill
    if(isContainsRect(savedPos, m_frame[3], m_frame[0], m_frame[1])) {
        setCursor(QCursor(Qt::SizeAllCursor));
        return;
    }
    setCursor(QCursor(Qt::ArrowCursor));
}
