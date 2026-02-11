#ifndef CQCamera3DAxes_H
#define CQCamera3DAxes_H

#include <CQCamera3DObject.h>

#include <CRGBA.h>

class CQCamera3DShaderProgram;
class CQCamera3DText;

class CQCamera3DAxes : public CQCamera3DObject {
 public:
  CQCamera3DAxes(CQCamera3DCanvas *canvas);

  bool isVisible() const { return isShowX() || isShowY() || isShowZ() || isShowGrid(); }

  bool isShowX() const { return showX_; }
  void setShowX(bool b) { showX_ = b; }

  bool isShowY() const { return showY_; }
  void setShowY(bool b) { showY_ = b; }

  bool isShowZ() const { return showZ_; }
  void setShowZ(bool b) { showZ_ = b; }

  bool isShowGrid() const { return showGrid_; }
  void setShowGrid(bool b) { showGrid_ = b; }

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
  bool showX_    { false };
  bool showY_    { false };
  bool showZ_    { false };
  bool showGrid_ { false };

  double xpos_ { 0.0 };
  double ypos_ { 0.0 };
  double zpos_ { 0.0 };

  double lineWidth_ { -1.0 };
  CRGBA  color_     { 0.5, 0.5, 0.5 };
};

#endif
