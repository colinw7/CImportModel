#ifndef CQCamera3DGrid_H
#define CQCamera3DGrid_H

#include <CQCamera3DObject.h>

#include <CRGBA.h>

class CQCamera3DShaderProgram;
class CQCamera3DText;

class CQCamera3DGrid : public CQCamera3DObject {
 public:
  CQCamera3DGrid(CQCamera3DCanvas *canvas);

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  void updateGeometry();

  void drawGeometry();

  CQCamera3DShaderProgram *shaderProgram() override;

 private:
  bool visible_ { false };
};

#endif
