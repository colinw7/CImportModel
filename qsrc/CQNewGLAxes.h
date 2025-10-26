#ifndef CQNewGLAxes_H
#define CQNewGLAxes_H

#include <CQNewGLObject.h>

class CQNewGLAxes : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLAxes(CQNewGLCanvas *canvas);

  void updateGeometry() override;

  void drawGeometry() override;

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  static CQNewGLShaderProgram *shaderProgram_;

  double lineWidth_ { -1.0 };
  CRGBA  color_     { 0.5, 0.5, 0.5 };
};

#endif
