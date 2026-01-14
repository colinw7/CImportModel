#ifndef CQCamera3DNormals_H
#define CQCamera3DNormals_H

#include <CQCamera3DObject.h>
#include <QColor>

class CQCamera3DCanvas;
class CQCamera3DObjectData;

class CQCamera3DNormals : public CQCamera3DObject {
 public:
  CQCamera3DNormals(CQCamera3DCanvas *canvas);

  double lineSize() const { return lineSize_; }
  void setLineSize(double r) { lineSize_ = r; }

  const QColor &lineColor() const { return lineColor_; }
  void setLineColor(const QColor &c) { lineColor_ = c; }

  bool isPointNormals() const { return pointNormals_; }
  void setPointNormals(bool b) { pointNormals_ = b; }

  bool isFaceNormals() const { return faceNormals_; }
  void setFaceNormals(bool b) { faceNormals_ = b; }

  //---

  void updateGeometry();

  void drawGeometry();

  //---

  CQCamera3DShaderProgram *shaderProgram() override;

 private:
  void addBufferNormals(CQCamera3DObjectData *object);

 protected:
  CQCamera3DCanvas* canvas_ { nullptr };

  bool   enabled_      { true };
  double lineSize_     { -1 };
  QColor lineColor_    { 255, 255, 255 };
  bool   pointNormals_ { false };
  bool   faceNormals_  { false };
};

#endif
