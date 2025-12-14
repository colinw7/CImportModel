#ifndef CQCamera3DUVMap_H
#define CQCamera3DUVMap_H

#include <QFrame>

class CQCamera3DApp;
class CGeomObject3D;

class CQCamera3DUVMap : public QFrame {
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

  void paintEvent(QPaintEvent *) override;

 private:
  CQCamera3DApp* app_         { nullptr };
  CGeomObject3D* object_      { nullptr };
  TextureType    textureType_ { TextureType::DIFFUSE };
  QColor         bgColor_     { 0, 0, 0 };
  QColor         lineColor_   { 255, 255, 255 };
};

#endif
