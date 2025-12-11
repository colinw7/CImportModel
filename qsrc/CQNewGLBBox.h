#ifndef CQNewGLBBox_H
#define CQNewGLBBox_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLCanvas;
class CQNewGLModel;

class CBBox3D;

class CQNewGLBBox : public CQNewGLObject {
 public:
  CQNewGLBBox(CQNewGLCanvas *canvas);

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private:
  void addBufferBBox(CQNewGLObject *object);

  void updateBufferBBox(CQNewGLObject *object, CBBox3D &bbox);

 protected:
  CQNewGLCanvas* canvas_ { nullptr };

  QColor color_ { 255, 255, 255 };
};

#endif
