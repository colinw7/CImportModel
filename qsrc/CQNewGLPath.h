#ifndef CQNewGLPath_H
#define CQNewGLPath_H

#include <CQNewGLObject.h>
#include <CGLPath3D.h>

class CQNewGLPath : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLPath(CQNewGLCanvas *canvas);

  const CGLPath3D &path() const { return path_; }
  void setPath(const CGLPath3D &v) { path_ = v; }

  void updateGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 protected:
  static CQNewGLShaderProgram* shaderProgram_;

  CGLPath3D path_;
};

#endif
