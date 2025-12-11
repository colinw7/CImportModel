#ifndef CQCamera3DShape_H
#define CQCamera3DShape_H

#include <CQCamera3DObject.h>
#include <CPlane3D.h>

class CQCamera3DShape : public CQCamera3DObject {
 public:
  CQCamera3DShape(CQCamera3DCanvas *canvas);

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

  CPlane3D leftPlane_, rightPlane_;
  CPlane3D topPlane_, bottomPlane_;
};

#endif
