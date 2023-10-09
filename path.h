#ifndef PATH_H
#define PATH_H

#include "geometrybase.h"

class Path : public GeometryBase
{
public:
    Path(const QVector<QPointF>& points =  QVector<QPointF>());
};

#endif // PATH_H
