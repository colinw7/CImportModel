#ifndef CQNewGLWireframe_H
#define CQNewGLWireframe_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLCanvas;
class CQNewGLModel;
class CGeomObject3D;

class CQNewGLWireframe : public CQNewGLObject {
 public:
  CQNewGLWireframe(CQNewGLCanvas *canvas);

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private:
  void addBufferWireframe(CQNewGLObject *object, int &offset);

 protected:
  CQNewGLCanvas* canvas_ { nullptr };

  QColor color_ { 255, 255, 255 };
};

#endif
