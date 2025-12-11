#ifndef CQCamera3DOverlay2D_H
#define CQCamera3DOverlay2D_H

#include <CQCamera3DObject.h>

class CQCamera3DOverlay2D : public CQCamera3DObject {
 public:
  CQCamera3DOverlay2D(CQCamera3DCanvas *canvas);

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;
};

#endif
