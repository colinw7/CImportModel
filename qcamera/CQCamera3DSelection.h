#ifndef CQCamera3DSelection_H
#define CQCamera3DSelection_H

#include <CQCamera3DObject.h>
#include <QColor>

class CQCamera3DCanvas;

class CSelection3D;

class CQCamera3DSelection : public CQCamera3DObject {
 public:
  CQCamera3DSelection(CQCamera3DCanvas *canvas);

  double lineWidth() const { return lineWidth_; }
  void setLineWidth(double r) { lineWidth_ = r; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; }

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 protected:
  CQCamera3DCanvas* canvas_ { nullptr };

  double lineWidth_ { -1.0 };

  QColor color_ { Qt::yellow };

  uint lineIndex_   { 0 };
  uint vertexIndex_ { 0 };
  uint endIndex_    { 0 };
};

#endif
