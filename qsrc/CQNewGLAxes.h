#ifndef CQNewGLAxes_H
#define CQNewGLAxes_H

#include <CQNewGLObject.h>

class CQNewGLText;

class CQNewGLAxes : public CQNewGLObject {
 public:
  static void initShader(CQNewGLCanvas *canvas);

  CQNewGLAxes(CQNewGLCanvas *canvas);

  void updateGeometry();

  void drawGeometry();

  CQNewGLShaderProgram *shaderProgram() override { return shaderProgram_; }

 private:
  using Texts = std::vector<CQNewGLText *>;

  static CQNewGLShaderProgram *shaderProgram_;

  Texts  texts_;
  double lineWidth_ { -1.0 };
  CRGBA  color_     { 0.5, 0.5, 0.5 };
};

#endif
