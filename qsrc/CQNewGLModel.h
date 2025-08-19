#ifndef CQNewGLModel_H
#define CQNewGLModel_H

#include <CMatrix3D.h>

#include <QFrame>

class CQNewGLToolbar;
class CQNewGLCanvas;
class CQNewGLControl;
class CQNewGLUVMap;
class QStackedWidget;

class CQNewGLModel : public QFrame {
  Q_OBJECT

 public:
  enum class Type {
    CAMERA = 0,
    MODEL  = 1,
    LIGHT  = 2
  };

 public:
  CQNewGLModel();

  const QString &buildDir() const { return buildDir_; }

  CQNewGLCanvas  *canvas () const { return canvas_; }
  CQNewGLControl *control() const { return control_; }
  CQNewGLUVMap   *uvMap  () const { return uvMap_; }

  int windowWidth() const { return windowWidth_; }
  void setWindowWidth(int i) { windowWidth_ = i; }

  int windowHeight() const { return windowHeight_; }
  void setWindowHeight(int i) { windowHeight_ = i; }

  double ambientStrength() const { return ambientStrength_; }
  void setAmbientStrength(double r) { ambientStrength_ = r; }

  double diffuseStrength() const { return diffuseStrength_; }
  void setDiffuseStrength(double r) { diffuseStrength_ = r; }

  double specularStrength() const { return specularStrength_; }
  void setSpecularStrength(double r) { specularStrength_ = r; }

  double emissiveStrength() const { return emissiveStrength_; }
  void setEmissiveStrength(double r) { emissiveStrength_ = r; }

  double shininess() const { return shininess_; }
  void setShininess(double r);

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  bool isShowUVMap() const { return showUVMap_; }
  void setShowUVMap(bool b);

  bool isShowBone() const { return showBone_; }
  void setShowBone(bool b);

  int boneInd() const { return boneInd_; }
  void setBoneInd(int i) { boneInd_ = i; }

  const QString &boneAnimName() const { return boneAnimName_; }
  void setBoneAnimName(const QString &s) { boneAnimName_ = s; }

  double boneAnimTime() const { return boneAnimTime_; }
  void setBoneAnimTime(double r) { boneAnimTime_ = r; }

  const CMatrix3D &boneMatrix() const { return boneMatrix_; }
  void setBoneMatrix(const CMatrix3D &m) { boneMatrix_ = m; }

  void update();
  void updateCamera();
  void updateLights();

 private:
  QString buildDir_;

  CQNewGLToolbar* toolbar_ { nullptr };
  CQNewGLCanvas*  canvas_  { nullptr };
  CQNewGLControl* control_ { nullptr };
  CQNewGLUVMap*   uvMap_   { nullptr };

  QStackedWidget* stack_ { nullptr };

  int windowWidth_  { 2000 };
  int windowHeight_ { 1500 };

  double ambientStrength_  { 0.3 };
  double diffuseStrength_  { 1.0 };
  double specularStrength_ { 0.2 };
  double emissiveStrength_ { 0.1 };

  double shininess_ { 32.0 };

  Type type_ { Type::CAMERA };

  bool showUVMap_ { false };
  bool showBone_  { false };

  int       boneInd_      { -1 };
  QString   boneAnimName_;
  double    boneAnimTime_ { 0.0 };
  CMatrix3D boneMatrix_   { CMatrix3D::identity() };
};

#endif
