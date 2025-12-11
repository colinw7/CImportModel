#ifndef CQNewGLAxes_H
#define CQNewGLAxes_H

#include <CQNewGLObject.h>

class CQNewGLWidget;
class CQNewGLModel;

class CQNewGLAxes : public CQNewGLObject {
 public:
  CQNewGLAxes(CQNewGLWidget *widget);

  CQNewGLShaderProgram *shaderProgram() override;

  void updateGeometry() override;

  void drawGeometry() override;

 private:
  double lineWidth_ { -1.0 };
  CRGBA  color_     { 0.5, 0.5, 0.5 };
};

#endif
