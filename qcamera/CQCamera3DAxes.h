#ifndef CQCamera3DAxes_H
#define CQCamera3DAxes_H

#include <CQCamera3DObject.h>

#include <CRGBA.h>

class CQCamera3DShaderProgram;
class CQCamera3DText;

class CQCamera3DAxes : public CQCamera3DObject {
 public:
  CQCamera3DAxes(CQCamera3DCanvas *canvas);

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  double xpos() const { return xpos_; }
  void setXPos(double r) { xpos_ = r; }

  double ypos() const { return ypos_; }
  void setYPos(double r) { ypos_ = r; }

  double zpos() const { return zpos_; }
  void setZPos(double r) { zpos_ = r; }

  void updateGeometry();

  void drawGeometry();

  CQCamera3DShaderProgram *shaderProgram() override;

 private:
  bool visible_ { false };

  double xpos_ { 0.0 };
  double ypos_ { 0.0 };
  double zpos_ { 0.0 };

  double lineWidth_ { -1.0 };
  CRGBA  color_     { 0.5, 0.5, 0.5 };
};

#endif
