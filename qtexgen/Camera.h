#ifndef Camera_H
#define Camera_H

#include <CGLCameraIFace.h>

#include <QObject>

namespace CQTextureGen {

class Camera : public QObject, public CGLCameraIFace {
  Q_OBJECT

 public:
  Camera();

 ~Camera() override;

  //---

  // set perspective values

  // field of view
  void setFov(double r) override;

  // pixel aspect
  void setAspect(double r) override;

  // near z
  void setNear(double r) override;

  // far z
  void setFar(double r) override;

  //---

  // origin (for rotation center)
  void setOrigin(const CVector3D &p) override;

  // position (for camera position)
  void setPosition(const CVector3D &p) override;

  //---

  // get perspective matrix
  CMatrix3DH perspectiveMatrix() const override;

  CMatrix3DH orthoMatrix() const override;

  // get view matrix
  CMatrix3DH viewMatrix() const override;

  //---

  CVector3D front() const override;
  CVector3D up   () const override;
  CVector3D right() const override;

  void moveAroundX(double) override { }
  void moveAroundY(double) override { }
  void moveAroundZ(double) override { }

 Q_SIGNALS:
  // send when state (data values) changed
  void stateChanged();
};

}

#endif
