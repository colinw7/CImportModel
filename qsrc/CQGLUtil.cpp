#include <CQGLUtil.h>
#include <CGLMatrix3D.h>

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

QVector3D
CQGLUtil::
toVector(const CGLVector3D &v)
{
  return QVector3D(v.x(), v.y(), v.z());
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
