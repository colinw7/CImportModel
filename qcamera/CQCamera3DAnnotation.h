#ifndef CQCamera3DAnnotation_H
#define CQCamera3DAnnotation_H

#include <CQCamera3DObject.h>

class CQCamera3DAnnotation : public CQCamera3DObject {
  Q_OBJECT

 public:
  CQCamera3DAnnotation(CQCamera3DCanvas *canvas);

  //---

  bool isWireframe() const { return wireframe_; }
  void setWireframe(bool b) { wireframe_ = b; }

  //---

  void drawGeometry();

  void drawMoveDirection();
  void drawCamera();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 public Q_SLOTS:
  void updateGeometry();

 private:
  bool wireframe_ { false };
};

#endif
