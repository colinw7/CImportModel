#ifndef CQNewGLHull_H
#define CQNewGLHull_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLHull : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLHull(CQNewGLCanvas *canvas);

  void updateGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  void addBufferHull(CQGLBuffer *hullBuffer);

 protected:
  static CQNewGLShaderProgram* shaderProgram_;

  QColor color_ { 255, 255, 255 };
};

#endif
