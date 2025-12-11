#ifndef CQCamera3DAnnotation_H
#define CQCamera3DAnnotation_H

#include <CQCamera3DObject.h>

class CQCamera3DAnnotation : public CQCamera3DObject {
 public:
  CQCamera3DAnnotation(CQCamera3DCanvas *canvas);

  //---

  bool isWireframe() const { return wireframe_; }
  void setWireframe(bool b) { wireframe_ = b; }

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 private:
  bool wireframe_ { true };
};

#endif
