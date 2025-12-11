#ifndef CQNewGLHull_H
#define CQNewGLHull_H

#include <CQNewGLObject.h>
#include <QColor>

class CQNewGLModel;
class CQNewGLCanvas;

class CHull3D;

class CQNewGLHull : public CQNewGLObject {
 public:
  CQNewGLHull(CQNewGLCanvas *canvas);

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 private:
  void addBufferHull(CQNewGLObject *object);

  void updateBufferHull(CQNewGLObject *object, CHull3D &hull);

 protected:
  CQNewGLCanvas* canvas_ { nullptr };

  QColor color_ { 255, 255, 255 };
};

#endif
