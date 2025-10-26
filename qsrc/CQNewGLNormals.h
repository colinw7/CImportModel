#ifndef CQNewGLNormals_H
#define CQNewGLNormals_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLNormals : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLNormals(CQNewGLCanvas *canvas);

  double lineSize() const { return lineSize_; }
  void setLineSize(double r) { lineSize_ = r; }

  const QColor &lineColor() const { return lineColor_; }
  void setLineColor(const QColor &c) { lineColor_ = c; }

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  void addBufferNormals(CQNewGLObject *object);

 protected:
  static CQNewGLShaderProgram* shaderProgram_;

  double lineSize_  { -1 };
  QColor lineColor_ { 255, 255, 255 };
};

#endif
