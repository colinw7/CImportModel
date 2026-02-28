#ifndef Camera_H
#define Camera_H

#include <CGLCameraIFace.h>

namespace CQShaderGen {

class Camera : public CGLCameraIFace {
 public:
  Camera();

 ~Camera();

  //---

  // get perspective matrix
  CMatrix3DH perspectiveMatrix() const override;

  // get orthonormal matrix
  CMatrix3DH orthoMatrix() const override;

  // get view matrix
  CMatrix3DH viewMatrix() const override;

  //---

  void getPixelRay(double x, double y, CPoint3D &rp1, CPoint3D &rp2) const;

  //---

  CVector3D front() const override;
  CVector3D up   () const override;
  CVector3D right() const override;

  // rotate around origin in x axis (pitch) - ZY
  void moveAroundX(double) override { }

  // rotate around origin in y axis (yaw) - XZ
  void moveAroundY(double) override { }

  // rotate around origin in z axis (roll) - XY
  void moveAroundZ(double) override { }

 private:
  void stateChanged() override { matrixValid_ = false; }

  void updateMatrices() const;

  void calcPerspectiveMatrix();
  void calcViewMatrix();

 private:
  bool matrixValid_ { false };

  CMatrix3DH perspectiveMatrix_;
  CMatrix3DH viewMatrix_;

  CMatrix3DH iperspectiveMatrix_;
  CMatrix3DH iviewMatrix_;
};

}

#endif
