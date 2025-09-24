#ifndef CQNewGLBBox_H
#define CQNewGLBBox_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLBBox : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLBBox(CQNewGLCanvas *canvas);

  void updateGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  void addBufferBBox(CQGLBuffer *bboxBuffer);

 protected:
  static CQNewGLShaderProgram* shaderProgram_;

  QColor color_ { 255, 255, 255 };
};

#endif
