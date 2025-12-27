#include <CQGLUtil.h>
#include <CGLMatrix3D.h>
#include <CMatrix3D.h>
#include <CMatrix3DH.h>
#include <CRGBA.h>
#include <QColor>

#include <GL/glut.h>

QMatrix4x4
CQGLUtil::
toQMatrix(const CGLMatrix3D &m)
{
  float values[16];

  memcpy(values, m.getData(), 16*sizeof(float));

  std::swap(values[ 1], values[ 4]);
  std::swap(values[ 2], values[ 8]);
  std::swap(values[ 3], values[12]);
  std::swap(values[ 6], values[ 9]);
  std::swap(values[ 7], values[13]);
  std::swap(values[11], values[14]);

  return QMatrix4x4(values);
}

QMatrix4x4
CQGLUtil::
toQMatrix(const CMatrix3D &m)
{
  auto *data = m.getData();

  float values[16];

  for (int i = 0; i < 16; ++i)
    values[i] = data[i];

  return QMatrix4x4(values);
}

QMatrix4x4
CQGLUtil::
toQMatrix(const CMatrix3DH &m)
{
  auto *data = m.getData();

  float values[16];

  for (int i = 0; i < 16; ++i)
    values[i] = data[i];

  return QMatrix4x4(values);
}

QVector3D
CQGLUtil::
toVector(const CGLVector3D &v)
{
  return QVector3D(v.x(), v.y(), v.z());
}

QVector3D
CQGLUtil::
toVector(const CVector3D &v)
{
  return QVector3D(v.x(), v.y(), v.z());
}

QVector3D
CQGLUtil::
toVector(const CPoint3D &p)
{
  return QVector3D(p.x, p.y, p.z);
}

QVector3D
CQGLUtil::
toVector(const QColor &c)
{
  return QVector3D(c.redF(), c.greenF(), c.blueF());
}

QVector3D
CQGLUtil::
toVector(const CRGBA &c)
{
  return QVector3D(c.getRed(), c.getGreen(), c.getBlue());
}

int
CQGLUtil::
QMouseButtonToGL(Qt::MouseButton button)
{
  int gl_button;

  switch (button) {
    case Qt::LeftButton:
      gl_button = GLUT_LEFT_BUTTON;
      break;
    case Qt::MiddleButton:
      gl_button = GLUT_MIDDLE_BUTTON;
      break;
    case Qt::RightButton:
      gl_button = GLUT_RIGHT_BUTTON;
      break;
    default:
      gl_button = GLUT_LEFT_BUTTON;
      break;
  }

  return gl_button;
}
