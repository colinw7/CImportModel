#ifndef CQNewGLBBox_H
#define CQNewGLBBox_H

#include <CQNewGLObject.h>
#include <QColor>

class CBBox3D;

class CQNewGLBBox : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLBBox(CQNewGLCanvas *canvas);

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  void addBufferBBox(CQNewGLObject *object);

  void updateBufferBBox(CQNewGLObject *object, CBBox3D &bbox);

 protected:
  static CQNewGLShaderProgram* shaderProgram_;

  QColor color_ { 255, 255, 255 };
};

#endif
