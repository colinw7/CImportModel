#ifndef CQCamera3DBBox_H
#define CQCamera3DBBox_H

#include <CQCamera3DObject.h>
#include <QColor>

class CQCamera3DCanvas;
class CGeomObject3D;

class CQCamera3DBBox : public CQCamera3DObject {
 public:
  CQCamera3DBBox(CQCamera3DCanvas *canvas);

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  //---

  bool isOriented() const { return oriented_; }
  void setOriented(bool b) { oriented_ = b; }

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 private:
  void addBufferOrientBBox(CGeomObject3D *object);
  void addBufferBBox(CGeomObject3D *object);

 protected:
  CQCamera3DCanvas* canvas_ { nullptr };

  QColor color_ { 255, 255, 255 };

  bool oriented_ { false };
};

#endif
