#ifndef CQNewGLDrawTree_H
#define CQNewGLDrawTree_H

#include <CQNewGLObject.h>

#include <CPoint3D.h>
#include <CVector3D.h>

#include <QObject>
#include <QImage>

class CQNewGLDrawTree : public QObject, public CQNewGLObject {
  Q_OBJECT

 public:
  static void initShader(CQNewGLCanvas *canvas);

  //---

  CQNewGLDrawTree(CQNewGLCanvas *canvas);

  //---

  bool isActive() const { return active_; }
  void setActive(bool b);

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

  const CVector3D &leftDirection() const { return leftDirection_; }
  void setLeftDirection(const CVector3D &v) { leftDirection_ = v; }

  const CVector3D &rightDirection() const { return rightDirection_; }
  void setRightDirection(const CVector3D &v) { rightDirection_ = v; }

  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  //---

  void addGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  static CQNewGLShaderProgram* shaderProgram_;

  bool active_ { false };

  bool      updateGeometry_ { true };
  CRGBA     color1_         { 0.32, 0.22, 0.18 };
  CRGBA     color2_         { 0.10, 0.80, 0.10 };
  double    width_          { 0.2 };
  double    height_         { 1.0 };
  CVector3D leftDirection_  {  2.0, 1.0, 0.0 };
  CVector3D rightDirection_ { -2.0, 1.0, 0.0 };
  int       depth_          { 6 };
};

#endif
