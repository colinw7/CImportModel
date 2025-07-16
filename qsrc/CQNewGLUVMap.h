#ifndef CQNewGLUVMap_H
#define CQNewGLUVMap_H

#include <QFrame>

class CQNewGLModel;
class CGeomObject3D;

class CQNewGLUVMap : public QFrame {
 public:
  enum class TextureType {
    NONE,
    DIFFUSE,
    NORMAL,
    SPECULAR,
    EMISSIVE
  };

 public:
  CQNewGLUVMap(CQNewGLModel *model);

  const CGeomObject3D *object() const { return object_; }
  void setObject(CGeomObject3D *p) { object_ = p; update(); }

  const TextureType &textureType() const { return textureType_; }
  void setTextureType(const TextureType &t) { textureType_ = t; update(); }

  void paintEvent(QPaintEvent *) override;

 private:
  CQNewGLModel*  model_       { nullptr };
  CGeomObject3D* object_      { nullptr };
  TextureType    textureType_ { TextureType::DIFFUSE };
};

#endif
