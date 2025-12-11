#ifndef CQNewGLNormals_H
#define CQNewGLNormals_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLCanvas;
class CQNewGLModel;

class CQNewGLNormals : public CQNewGLObject {
 public:
  CQNewGLNormals(CQNewGLCanvas *canvas);

  double lineSize() const { return lineSize_; }
  void setLineSize(double r) { lineSize_ = r; }

  const QColor &lineColor() const { return lineColor_; }
  void setLineColor(const QColor &c) { lineColor_ = c; }

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private:
  void addBufferNormals(CQNewGLObject *object);

 protected:
  CQNewGLCanvas* canvas_ { nullptr };

  double lineSize_  { -1 };
  QColor lineColor_ { 255, 255, 255 };
};

#endif
