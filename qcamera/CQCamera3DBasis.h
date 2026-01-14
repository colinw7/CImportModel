#ifndef CQCamera3DBasis_H
#define CQCamera3DBasis_H

#include <CQCamera3DObject.h>
#include <QColor>

class CQCamera3DObjectData;
class CQCamera3DCanvas;

class CBasis3D;

class CQCamera3DBasis : public CQCamera3DObject {
 public:
  CQCamera3DBasis(CQCamera3DCanvas *canvas);

  bool isShow() const { return show_; }
  void setShow(bool b) { show_ = b; }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  double lineWidth() const { return lineWidth_; }
  void setLineWidth(double r) { lineWidth_ = r; }

  double lineSize() const { return lineSize_; }
  void setLineSize(double r) { lineSize_ = r; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 protected:
  CQCamera3DCanvas* canvas_ { nullptr };

  bool show_  { false };
  bool valid_ { false };

  CQCamera3DObjectData* objectData_ { nullptr };

  double lineWidth_ { -1.0 };
  double lineSize_  { -1.0 };

  QColor color_ { 200, 200, 200 };
};

#endif
