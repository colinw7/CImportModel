#ifndef CQNewGLUtil_H
#define CQNewGLUtil_H

#include <GL/glu.h>

namespace {

inline CGLVector3D QColorToVector(const QColor &c) {
  return CGLVector3D(c.red()/255.0, c.green()/255.0, c.blue()/255.0);
}

inline CGLVector3D ColorToVector(const CRGBA &c) {
  return CGLVector3D(c.getRed(), c.getGreen(), c.getBlue());
}

inline CRGBA QColorToRGBA(const QColor &c) {
  return CRGBA(c.red()/255.0, c.green()/255.0, c.blue()/255.0);
}

inline QColor RGBAToQColor(const CRGBA &c) {
  return QColor(int(255*c.getRed()), int(255*c.getGreen()), int(255*c.getBlue()));
}

inline CGLVector3D vectorToGLVector(const CVector3D &v) {
  return CGLVector3D(v.getX(), v.getY(), v.getZ());
}

inline QVector3D pointToQVector(const CPoint3D &p) {
  return QVector3D(p.getX(), p.getY(), p.getZ());
}

inline QVector3D vectorToQVector(const CVector3D &v) {
  return QVector3D(v.getX(), v.getY(), v.getZ());
}

inline double max3(double x, double y, double z) {
  return std::max(std::max(x, y), z);
}

inline bool checkError(const char *op) {
  // check GL error generated
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << op << " = " << gluErrorString(err) << "\n";
    return false;
  }

  return true;
}

inline const char *toCString(const QString &str) {
  static char cString[256];
  assert(str.length() < 255);
  strcpy(cString, str.toStdString().c_str());
  return cString;
}

}

#endif
