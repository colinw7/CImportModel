#ifndef CQCamera3DMouseMode_H
#define CQCamera3DMouseMode_H

#include <CQCamera3DCanvas.h>

#include <CGeomCircle3D.h>
#include <CGeomSphere3D.h>

#include <vector>

class CQCamera3DWidget;
class CQCamera3DMouseModeMgr;

class CGeomObject3D;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

class CQCamera3DMouseMode {
 public:
  CQCamera3DMouseMode() { }

  virtual ~CQCamera3DMouseMode() { }

  const CQCamera3DMouseModeMgr *mgr() const { return mgr_; }
  void setMgr(CQCamera3DMouseModeMgr *p) { mgr_ = p; }

  virtual void begin() { }
  virtual void end  () { }

  virtual void mousePress  (QMouseEvent *) { }
  virtual void mouseMove   (QMouseEvent *) { }
  virtual void mouseRelease(QMouseEvent *) { }

  virtual void wheelEvent(QWheelEvent *) { }

  virtual void keyPress(QKeyEvent *) { }

 private:
  CQCamera3DMouseModeMgr* mgr_ { nullptr };
};

//---

class CQCamera3DMouseModeMgr {
 public:
  CQCamera3DMouseModeMgr(CQCamera3DWidget *widget);

  CQCamera3DWidget *widget() const { return widget_; }

  int depth() const { return mouseModeStack_.size(); }

  CQCamera3DMouseMode *currentMode();

  void startMode(CQCamera3DMouseMode *mode);

  CQCamera3DMouseMode *endMode();

 private:
  using MouseModeStack = std::vector<CQCamera3DMouseMode *>;

  CQCamera3DWidget* widget_ { nullptr };
  MouseModeStack    mouseModeStack_;
};

//---

class CQCamera3DAddMouseMode : public CQCamera3DMouseMode {
 public:
  using AddType = CQCamera3DCanvas::AddObjectType;

 public:
  CQCamera3DAddMouseMode(const AddType &type, CGeomObject3D *object);

  const AddType &type() const { return type_; }

  CGeomObject3D *object() const { return object_; }

  void begin() override;
  void end  () override;

  void updateObject();

  const CPoint3D &position() const { return position_; }
  void setPosition(const CPoint3D &v) { position_ = v; }

  const CVector3D &rotation() const { return rotation_; }
  void setRotation(const CVector3D &v) { rotation_ = v; }

  const CVector3D &size() const { return size_; }
  void setSize(const CVector3D &v) { size_ = v; }

  double torusRadius() const { return torusRadius_; }
  void setTorusRadius(double r) { torusRadius_ = r; }

  const CGeomCircle3D::ConfigData &circleConfig() const { return circleConfig_; }
  void setCircleConfig(const CGeomCircle3D::ConfigData &config) { circleConfig_ = config; };

  const CGeomSphere3D::ConfigData &sphereConfig() const { return sphereConfig_; }
  void setSphereConfig(const CGeomSphere3D::ConfigData &config) { sphereConfig_ = config; };

 private:
  AddType        type_   { AddType::NONE };
  CGeomObject3D* object_ { nullptr };

  CPoint3D  position_ { 0, 0, 0 };
  CVector3D rotation_ { 0, 0, 0 };
  CVector3D size_     { 1, 1, 1 };

  double power1_      { 1.0 };
  double power2_      { 1.0 };
  double torusRadius_ { 0.1 };

  CGeomCircle3D::ConfigData circleConfig_;
  CGeomSphere3D::ConfigData sphereConfig_;
};

//---

class CQCamera3DMoveMouseMode : public CQCamera3DMouseMode {
 public:
  CQCamera3DMoveMouseMode(CGeomObject3D *object);

  CGeomObject3D *object() const { return object_; }

  void begin() override;
  void end  () override;

  void keyPress(QKeyEvent *) override;

 private:
  CGeomObject3D* object_ { nullptr };
};

//---

class CQCamera3DScaleMouseMode : public CQCamera3DMouseMode {
 public:
  CQCamera3DScaleMouseMode(CGeomObject3D *object);

  CGeomObject3D *object() const { return object_; }

  void begin() override;
  void end  () override;

  void keyPress(QKeyEvent *) override;

 private:
  CGeomObject3D* object_ { nullptr };
};

//---

class CQCamera3DRotateMouseMode : public CQCamera3DMouseMode {
 public:
  CQCamera3DRotateMouseMode(CGeomObject3D *object);

  CGeomObject3D *object() const { return object_; }

  void begin() override;
  void end  () override;

  void keyPress(QKeyEvent *) override;

 private:
  CGeomObject3D* object_ { nullptr };
};


//---

class CQCamera3DExtrudeMouseMode : public CQCamera3DMouseMode {
 public:
  CQCamera3DExtrudeMouseMode();

  void begin() override;
  void end  () override;

  void keyPress(QKeyEvent *) override;
};

#endif
