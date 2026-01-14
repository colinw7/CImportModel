#ifndef CQCamera3DBillboard_H
#define CQCamera3DBillboard_H

#include <CQCamera3DObject.h>

class CQCamera3DBillboard : public CQCamera3DObject {
 public:
  CQCamera3DBillboard(CQCamera3DCanvas *canvas);

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 private:
  void addCursor();

 private:
  CQGLBuffer* buffer_ { nullptr };
};

#endif
