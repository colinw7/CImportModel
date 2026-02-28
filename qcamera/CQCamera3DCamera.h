#ifndef CQCamera3DCamera_H
#define CQCamera3DCamera_H

#include <CGLCameraIFace.h>

#include <QObject>

/*
 * rotate by yaw, followed by pitch, followed by roll
 *
 * starting with vector (1, 0, 0), look down negative z axis is yaw=-90 degrees, pitch=0, roll=0
 *
 *  pitch : rotate around right
 *  yaw   : rotate around up
 *  roll  : rotate around front
 *
 * OpenGL uses a right-handed coordinate system where:
 *   +X is right, +Y is up, and +Z points out of the screen towards the viewer
 */
class CQCamera3DCamera : public QObject, public CGLCameraIFace {
  Q_OBJECT

 public:
  CQCamera3DCamera();

 ~CQCamera3DCamera() override;

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

#if 0
  void setOriginXY(const CVector3D &p);
  void setOriginYZ(const CVector3D &p);
  void setOriginXZ(const CVector3D &p);
#endif

  //---

  // rotation x angle
  void setPitch(double r) override;

  // rotation x angle
  void setYaw(double r) override;

  // rotation z angle
  void setRoll(double r) override;

  //---

  CVector3D front() const override;
  CVector3D up   () const override;
  CVector3D right() const override;

  //---

  // get perspective matrix
  CMatrix3DH perspectiveMatrix() const override;

  // get ortho matrix
  CMatrix3DH orthoMatrix() const override;

  // get view matrix
  CMatrix3DH viewMatrix() const override;

  //---

  // rotate around origin in x axis (pitch) - ZY
  void moveAroundX(double d) override;

  // rotate around origin in y axis (yaw) - XZ
  void moveAroundY(double d) override;

  // rotate around origin in z axis (roll) - XY
  void moveAroundZ(double d) override;

 public:
  // get view matrix
  CMatrix3DH viewRotationMatrix() const;

  CMatrix3DH calcRotationMatrix() const;

  CVector3D viewRotationPosition() const;
#if 0
  CVector3D unrollPoint(const CVector3D &p) const;

  CVector3D rollPoint(const CVector3D &p) const;

  CVector3D unrollYawPoint(const CVector3D &p) const;

  CVector3D rollYawPoint(const CVector3D &p) const;

  CVector3D unrollYawPitchPoint(const CVector3D &p) const;
#endif

  struct Shape {
    CPoint3D p11;
    CPoint3D p12;
    CPoint3D p21;
    CPoint3D p22;
  };

  void getCameraShape(Shape &shape) const;

 private:
  void stateChanged() override { Q_EMIT stateChangedSignal(); }

  // given euler angles calc front/up/right vectors
  void calcVectors();

  // given position and origin calc euler angles
  void calcAngles();

 Q_SIGNALS:
  // sent when state (data values) changed
  void stateChangedSignal();

 private:
  bool matrixValid_ { false };

  CMatrix3DH rotationMatrix_;

  CVector3D front_;
  CVector3D up_;
  CVector3D right_;
};

#endif
