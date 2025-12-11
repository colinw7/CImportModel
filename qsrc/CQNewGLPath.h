#ifndef CQNewGLPath_H
#define CQNewGLPath_H

#include <CQNewGLObject.h>
#include <CGLPath3D.h>

class CQNewGLCanvas;
class CQNewGLModel;

class CQNewGLPath : public CQNewGLObject {
 public:
  CQNewGLPath(CQNewGLCanvas *canvas);

  const CGLPath3D &path() const { return path_; }
  void setPath(const CGLPath3D &v) { path_ = v; }

  //---

  void updateGeometry() override;

  void drawGeometry() override;

  //---

  CQNewGLShaderProgram *shaderProgram() override;

 protected:
  CQNewGLCanvas* canvas_ { nullptr };

  CGLPath3D path_;
};

#endif
