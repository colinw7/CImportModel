#ifndef Camera_H
#define Camera_H

#include <CGLCameraIFace.h>

namespace CQShaderGen {

class Camera {
 public:
  Camera();

 ~Camera();

  //---

  // set perspective values

  // field of view
  double fov() const { return fov_; }
  void setFov(double r) { fov_ = r; matrixValid_ = false; }

  // pixel aspect
  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; matrixValid_ = false; }

  // near z
  double near() const { return near_; }
  void setNear(double r) { near_ = r; matrixValid_ = false; }

  // far z
  double far() const { return far_; }
  void setFar(double r) { far_ = r; matrixValid_ = false; }

  //---

  // origin (for rotation center)
  const CVector3D &origin() const { return origin_; }
  void setOrigin(const CVector3D &p) { origin_ = p; matrixValid_ = false; }

  // position (for camera position)
  const CVector3D &position() const { return position_; }
  void setPosition(const CVector3D &p) { position_ = p; matrixValid_ = false; }

  //---

  // get perspective matrix
  CMatrix3DH perspectiveMatrix() const;

  // get view matrix
  CMatrix3DH viewMatrix() const;

  //---

  void getPixelRay(double x, double y, CPoint3D &rp1, CPoint3D &rp2) const;

  //---

  CVector3D front() const;
  CVector3D up   () const;
  CVector3D right() const;

 private:
  void updateMatrices() const;

  void calcPerspectiveMatrix();
  void calcViewMatrix();

 private:
  double fov_    { 70.0 };   // field of view
  double aspect_ { 1.0 };    // pixel aspect
  double near_   { 0.01 };   // near z (too small get z fighting)
  double far_    { 1000.0 }; // far z (too large (too far fron near) get z fighting)

  CVector3D origin_   { 0.0, 0.0, 0.0 }; // camera rotation center
  CVector3D position_ { 1.0, 1.0, 1.0 }; // camera position

  bool matrixValid_ { false };

  CMatrix3DH perspectiveMatrix_;
  CMatrix3DH viewMatrix_;

  CMatrix3DH iperspectiveMatrix_;
  CMatrix3DH iviewMatrix_;
};

}

#endif
