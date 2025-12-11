#ifndef CQGL_UTIL_H
#define CQGL_UTIL_H

#include <QMatrix4x4>
#include <QVector3D>
#include <Qt>

class CGLMatrix3D;
class CGLVector3D;
class CMatrix3D;
class CMatrix3DH;
class CVector3D;
class CRGBA;

namespace CQGLUtil {

QMatrix4x4 toQMatrix(const CGLMatrix3D &m);
QMatrix4x4 toQMatrix(const CMatrix3D &m);
QMatrix4x4 toQMatrix(const CMatrix3DH &m);

QVector3D toVector(const CGLVector3D &v);
QVector3D toVector(const CVector3D &v);
QVector3D toVector(const QColor &c);
QVector3D toVector(const CRGBA &c);

int QMouseButtonToGL(Qt::MouseButton button);

}

#endif
