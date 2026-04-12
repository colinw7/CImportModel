#ifndef CQCamera3DLight_H
#define CQCamera3DLight_H

#include <CGeomLight3D.h>
#include <CQCamera3DCamera.h>

#include <QObject>

class CQCamera3DLight : public CQCamera3DCamera, public CGeomLight3D {
  Q_OBJECT

 public:
  CQCamera3DLight(CGeomScene3D *scene, const std::string &name="");

  uint id() const { return CGeomLight3D::id(); }
  void setId(uint id) { CGeomLight3D::setId(id); }

  std::string name() const { return CGeomLight3D::name(); }

  //---

  void setPosition(const CPoint3D &point) override {
    CQCamera3DCamera::setPosition(CVector3D(point));
    CGeomLight3D::setPosition(point);
  }

  const CVector3D &getDirection() const override {
    static CVector3D dir;
    dir = CQCamera3DCamera::front();
    return dir;
  }

  void setDirection(const CVector3D &dir) override {
    CQCamera3DCamera::setFront(dir.normalized());
  }

#if 0
  void setVisible(bool b) override { CGLCameraIFace::setVisible(b); stateChanged(); }

  //---

  void setOrigin(const CVector3D &p) override {
    CGLCameraIFace::setOrigin(p); stateChanged();
  }

  // light position
  const CPoint3D &getPosition() const override { return CGeomLight3D::getPosition(); }

  void setPosition(const CPoint3D &pos) override {
    CGeomLight3D::setPosition(pos);
    CGLCameraIFace::setPosition(CVector3D(pos));

    stateChanged();
  }

  // camera position
  void setPosition(const CVector3D &pos) override {
    setPosition(pos.point());
  }

  //---

  CVector3D front() const override { constCalcVectors(); return front_; }
  CVector3D up   () const override { constCalcVectors(); return up_   ; }
  CVector3D right() const override { constCalcVectors(); return right_; }

  CMatrix3DH perspectiveMatrix() const override {
    return CMatrix3DH::perspective(fov(), aspect(), near(), far());
  }

  CMatrix3DH orthoMatrix() const override {
    auto pov = CVector3D(CVector3D(getPosition()), origin());
    auto r   = pov.length()/2.0;

    return CMatrix3DH::ortho(/*left*/-r, /*right*/r, /*bottom*/-r, /*top*/r, near(), far());
  }

  CMatrix3DH viewMatrix() const override {
    constCalcVectors();

    CMatrix3DH m;
    m.setLookAt(getPosition(), front_, up_, right_);

    return m;
  }

  void stateChanged() override { Q_EMIT stateChangedSignal(); }

 Q_SIGNALS:
  void stateChangedSignal();

 private:
  void constCalcVectors() const {
    const_cast<CQCamera3DLight *>(this)->calcVectors();
  }

  void calcVectors();

 private:
  CVector3D front_ { 0, 0, 1 };
  CVector3D up_    { 0, 1, 0 };
  CVector3D right_ { 1, 0, 0 };
#endif
};

#endif
