#ifndef CQNewGLModel_H
#define CQNewGLModel_H

#include <CGeom3DType.h>
#include <CGLVector3D.h>
#include <CMatrix3D.h>
#include <CBBox3D.h>

#include <QFrame>

class CQNewGLToolbar;
class CQNewGLStatusBar;
class CQNewGLCanvas;
class CQNewGLUVMap;
class CQNewGLBones;
class CQNewGLFractalWidget;
class CQNewGLControl;
class CQNewGLUVControl;
class CQNewGLFractalControl;
class CQNewGLWidget;

class CGeomObject3D;
class CGeomScene3D;

class QTabWidget;
class QStackedWidget;

class CQNewGLModel : public QFrame {
  Q_OBJECT

 public:
  enum class Type {
    CAMERA = 0,
    MODEL  = 1,
    LIGHT  = 2
  };

  struct LoadData {
    bool invertX { false };
    bool invertY { false };
    bool invertZ { false };
    bool swapYZ  { false };

    std::string fileNameMap;

    std::vector<CGeomObject3D *> objects;

    LoadData() { }
  };

 public:
  CQNewGLModel();

  const QString &buildDir() const { return buildDir_; }

  CQNewGLToolbar   *toolbar  () const { return toolbar_; }
  CQNewGLStatusBar *statusBar() const { return statusBar_; }

  CQNewGLCanvas         *canvas      () const { return canvas_; }
  CQNewGLUVMap          *uvMap       () const { return uvMap_; }
  CQNewGLBones          *bones       () const { return bones_; }
  CQNewGLFractalWidget* fractalWidget() const { return fractalWidget_; }

  QStackedWidget        *controlStack  () const { return controlStack_; }
  CQNewGLControl        *control       () const { return control_; }
  CQNewGLUVControl      *uvControl     () const { return uvControl_; }
  CQNewGLFractalControl *fractalControl() const { return fractalControl_; }

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

  //---

  // current bone
  bool isShowBone() const { return showBone_; }
  void setShowBone(bool b);

  int boneInd() const { return boneData_.ind; }
  void setBoneInd(int i) { boneData_.ind = i; }

  const QString &boneAnimName() const { return boneData_.animName; }
  void setBoneAnimName(const QString &s) { boneData_.animName = s; }

  double boneAnimTime() const { return boneData_.animTime; }
  void setBoneAnimTime(double r) { boneData_.animTime = r; }

  const CMatrix3D &boneMatrix() const { return boneData_.matrix; }
  void setBoneMatrix(const CMatrix3D &m) { boneData_.matrix = m; }

  //---

  bool isTimerRunning() const { return timerRunning_; }
  void setTimerRunning(bool b);

  void startTimer();
  void stopTimer();

  double time() const { return time_; }
  void setTime(double t) { time_ = t; }

  //---

  bool isAnimEnabled() const { return animEnabled_; }
  void setAnimEnabled(bool b) { animEnabled_ = b; }

  const QString &animName() const { return animName_; }
  void setAnimName(const QString &s) { animName_ = s; }

  //---

  void update();
  void updateCamera();
  void updateLights();

  //---

  CGeomScene3D* getScene() const { return scene_; }

  bool loadModel(const QString &modelName, CGeom3DType format, LoadData &loadData);

  //---

  CBBox3D getObjectBBox(CGeomObject3D *object) const;
  CBBox3D getModelBBox(CGeomObject3D *object) const;

  CBBox3D transformBBox(const CBBox3D &bbox, const CMatrix3D &matrix) const;

  CMatrix3D getMeshGlobalTransform(CGeomObject3D *object, bool invert) const;

 private:
  void addWidgets();

 Q_SIGNALS:
  void timerStep();

  void modelAdded();

 public Q_SLOTS:
  void timerSlot();

 private Q_SLOTS:
  void tabSlot(int i);

 private:
  QString buildDir_;

  // widgets
  CQNewGLToolbar*   toolbar_   { nullptr };
  CQNewGLStatusBar* statusBar_ { nullptr };

  QTabWidget* tab_ { nullptr };

  CQNewGLCanvas*        canvas_        { nullptr };
  CQNewGLUVMap*         uvMap_         { nullptr };
  CQNewGLBones*         bones_         { nullptr };
  CQNewGLFractalWidget* fractalWidget_ { nullptr };

  QStackedWidget*        controlStack_   { nullptr };
  CQNewGLControl*        control_        { nullptr };
  CQNewGLUVControl*      uvControl_      { nullptr };
  CQNewGLFractalControl* fractalControl_ { nullptr };

  //---

  // globals
  double ambientStrength_  { 0.3 };
  double diffuseStrength_  { 1.0 };
  double specularStrength_ { 0.2 };
  double emissiveStrength_ { 0.1 };

  double shininess_ { 32.0 };

  // timer
  QTimer* timer_        { nullptr };
  bool    timerRunning_ { false };
  uint    ticks_        { 0 };
  double  time_         { 0.0 };

  // state
  Type type_ { Type::CAMERA };

  // bones
  bool showBone_ { false };

  struct BoneData {
    int       ind      { -1 };
    QString   animName;
    double    animTime { 0.0 };
    CMatrix3D matrix   { CMatrix3D::identity() };
  };

  BoneData boneData_;

  // anim
  bool    animEnabled_ { false };
  QString animName_;

  //---

  // object data
  CGeomScene3D* scene_ { nullptr };
};

#endif
