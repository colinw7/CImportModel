#ifndef CQGL_UTIL_H
#define CQGL_UTIL_H

#include <QMatrix4x4>
#include <QVector3D>
#include <Qt>

class CGLMatrix3D;
class CGLVector3D;

namespace CQGLUtil {

QMatrix4x4 toQMatrix(const CGLMatrix3D &m);

QVector3D toVector(const CGLVector3D &v);

int QMouseButtonToGL(Qt::MouseButton button);

}

#endif
