#ifndef CQCamera3DUVMap_H
#define CQCamera3DUVMap_H

#include <CPoint2D.h>

#include <QFrame>

class CQCamera3DApp;

class CGeomObject3D;
class CGeomFace3D;

class CQCamera3DUVMap : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool wrapValues READ isWrapValues WRITE setWrapValues)

 public:
  enum class TextureType {
    NONE,
    DIFFUSE,
    NORMAL,
    SPECULAR,
    EMISSIVE
  };

 public:
  CQCamera3DUVMap(CQCamera3DApp *model);

  const CGeomObject3D *object() const { return object_; }
  void setObject(CGeomObject3D *p) { object_ = p; update(); }

  const TextureType &textureType() const { return textureType_; }
  void setTextureType(const TextureType &t) { textureType_ = t; update(); }

  bool isWrapValues() const { return wrapValues_; }
  void setWrapValues(bool b) { wrapValues_ = b; update(); }

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent (QMouseEvent *) override;

 private:
  double mapU(double u) { return (w_ - 1)*(u - umin_)/(umax_ - umin_); }
  double mapV(double v) { return (h_ - 1)*(v - vmin_)/(vmax_ - vmin_); }

  double unmapU(double u) { return u*(umax_ - umin_)/(w_ - 1) + umin_; }
  double unmapV(double v) { return v*(vmax_ - vmin_)/(h_ - 1) + vmin_; }

 private:
  CQCamera3DApp* app_         { nullptr };
  CGeomObject3D* object_      { nullptr };
  TextureType    textureType_ { TextureType::DIFFUSE };
  QColor         bgColor_     { 0, 0, 0 };
  QColor         lineColor_   { 255, 255, 255 };
  bool           wrapValues_  { false };

  mutable double umin_ { 0.0 };
  mutable double umax_ { 1.0 };
  mutable double vmin_ { 0.0 };
  mutable double vmax_ { 1.0 };

  mutable int w_ { 100 };
  mutable int h_ { 100 };

  struct TexturePoint {
    CGeomFace3D* face { nullptr };
    int          ind  { 0 };
    CPoint2D     tpoint;
    QPointF      p;
  };

  std::vector<TexturePoint> texturePoints_;

  CGeomFace3D* selectedFace_ { nullptr };
  int          selectedInd_  { -1 };
};

#endif
