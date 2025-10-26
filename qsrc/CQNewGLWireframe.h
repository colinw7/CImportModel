#ifndef CQNewGLWireframe_H
#define CQNewGLWireframe_H

#include <CQNewGLObject.h>
#include <QColor>

class CGeomObject3D;

class CQNewGLWireframe : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLWireframe(CQNewGLCanvas *canvas);

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  void addBufferWireframe(CQNewGLObject *object, int &offset);

 protected:
  static CQNewGLShaderProgram* shaderProgram_;

  QColor color_ { 255, 255, 255 };
};

#endif
