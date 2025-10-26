#ifndef CQNewGLBasis_H
#define CQNewGLBasis_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLModelObject;
class CBasis3D;

class CQNewGLBasis : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLBasis(CQNewGLCanvas *canvas);

  bool isShow() const { return show_; }
  void setShow(bool b) { show_ = b; }

  double lineWidth() const { return lineWidth_; }
  void setLineWidth(double r) { lineWidth_ = r; }

  double lineSize() const { return lineSize_; }
  void setLineSize(double r) { lineSize_ = r; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 protected:
  static CQNewGLShaderProgram* shaderProgram_;

  bool show_ { false };

  CQNewGLModelObject* objectData_ { nullptr };

  double lineWidth_ { -1.0 };
  double lineSize_  { -1.0 };

  QColor color_ { 200, 200, 200 };
};

#endif
