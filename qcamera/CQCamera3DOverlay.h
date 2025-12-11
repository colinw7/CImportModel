#ifndef CQCamera3DOverlay_H
#define CQCamera3DOverlay_H

#include <CQCamera3DObject.h>

class CQCamera3DOverlay : public CQCamera3DObject {
 public:
  CQCamera3DOverlay(CQCamera3DCanvas *canvas);

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;
};

#endif
