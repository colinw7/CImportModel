#include <CQCamera3DNewCamera.h>

#include <cmath>

static CVector3D CAMERA_WORLD_FORWARD = CVector3D(0.0, 0.0, -1.0);
static CVector3D CAMERA_WORLD_UP      = CVector3D(0.0, 1.0,  0.0);
static CVector3D CAMERA_WORLD_RIGHT   = CVector3D(1.0, 0.0,  0.0);

CQCamera3DNewCamera::
CQCamera3DNewCamera()
{
  updateOrientation();
}

//---

const CVector3D &
CQCamera3DNewCamera::
origin() const
{
  updateOrientation();

  return CGLCameraIFace::origin();
}

void
CQCamera3DNewCamera::
setOrigin(const CVector3D &p)
{
  CGLCameraIFace::setOrigin(p);
}

const CVector3D &
CQCamera3DNewCamera::
position() const
{
  updateOrientation();

  return CGLCameraIFace::position();
}

void
CQCamera3DNewCamera::
setPosition(const CVector3D &p)
{
  updateOrientation();

  setOrigin(p + front_*distance_);
}

//---

double
CQCamera3DNewCamera::
pitch() const
{
  updateOrientation();

  return CGLCameraIFace::pitch();
}

void
CQCamera3DNewCamera::
setPitch(double r)
{
  rotateX(r - pitch());
}

double
CQCamera3DNewCamera::
yaw() const
{
  updateOrientation();

  return CGLCameraIFace::yaw();
}

void
CQCamera3DNewCamera::
setYaw(double r)
{
  rotateY(r - yaw());
}

double
CQCamera3DNewCamera::
roll() const
{
  updateOrientation();

  return CGLCameraIFace::roll();
}

void
CQCamera3DNewCamera::
setRoll(double r)
{
  rotateZ(r - roll());
}

//---

CVector3D
CQCamera3DNewCamera::
front() const
{
  updateOrientation();

  return front_;
}

CVector3D
CQCamera3DNewCamera::
up() const
{
  updateOrientation();

  return up_;
}

CVector3D
CQCamera3DNewCamera::
right() const
{
  updateOrientation();

  return right_;
}

void
CQCamera3DNewCamera::
moveRight(double d)
{
  originDelta_.setX(originDelta_.x() + d);

  stateChanged();
}

void
CQCamera3DNewCamera::
moveUp(double d)
{
  originDelta_.setY(originDelta_.y() + d);

  stateChanged();
}

void
CQCamera3DNewCamera::
moveFront(double d)
{
  originDelta_.setZ(originDelta_.z() + d);

  stateChanged();
}

void
CQCamera3DNewCamera::
rotateX(double da)
{
  angleDelta_.setX(angleDelta_.x() + da);

  stateChanged();
}

void
CQCamera3DNewCamera::
rotateY(double da)
{
  angleDelta_.setY(angleDelta_.y() + da);

  stateChanged();
}

void
CQCamera3DNewCamera::
rotateZ(double da)
{
  angleDelta_.setZ(angleDelta_.z() + da);

  stateChanged();
}

void
CQCamera3DNewCamera::
updateOrientation() const
{
  if (orientationValid_)
    return;

  const_cast<CQCamera3DNewCamera *>(this)->updateOrientationI();
}

void
CQCamera3DNewCamera::
updateOrientationI()
{
  orientationValid_ = false;

  //---

  auto angles = toEuler(orientation_);

  pitch_ = angles.x();
  yaw_   = angles.y();
  roll_  = angles.z();

  if (isClampPitch() || isClampYaw() || isClampRoll()) {
    if (isClampPitch()) {
      angleDelta_.setX(std::max(minPitch_ - pitch_, angleDelta_.x()));
      angleDelta_.setX(std::min(maxPitch_ - pitch_, angleDelta_.x()));
    }

    if (isClampYaw()) {
      angleDelta_.setY(std::max(minYaw_ - yaw_, angleDelta_.y()));
      angleDelta_.setY(std::min(maxYaw_ - yaw_, angleDelta_.y()));
    }

    if (isClampRoll()) {
      angleDelta_.setZ(std::max(minRoll_ - roll_, angleDelta_.z()));
      angleDelta_.setZ(std::min(maxRoll_ - roll_, angleDelta_.z()));
    }

    pitch_ += angleDelta_.x();
    yaw_   += angleDelta_.y();
    roll_  += angleDelta_.z();
  }

  //---

  auto qp = CQuaternion::angleAxis(angleDelta_.x(), CAMERA_WORLD_RIGHT); // pitch
  auto qy = CQuaternion::angleAxis(angleDelta_.y(), CAMERA_WORLD_UP   ); // yaw

  if (isDisableRoll()) {
    orientation_ = orientation_*qp;
    orientation_ = qy*orientation_;
  }
  else {
    auto qr = CQuaternion::angleAxis(angleDelta_.z(), CAMERA_WORLD_FORWARD); // roll

    orientation_ = orientation_*qp;
    orientation_ = orientation_*qy;
    orientation_ = orientation_*qr;
  }

  orientation_.normalize();

  angleDelta_ = CVector3D();

  //---

  auto iorientation = orientation_.conjugated();

  front_ = CAMERA_WORLD_FORWARD*iorientation;
  up_    = CAMERA_WORLD_UP     *iorientation;
  right_ = CAMERA_WORLD_RIGHT  *iorientation;

  origin_ += right_*originDelta_.x();
  origin_ += up_   *originDelta_.y();
  origin_ += front_*originDelta_.z();

  originDelta_ = CVector3D();

  position_ = origin_ - front_*distance_;

  calcPerspectiveMatrix();

  calcViewMatrix();
}

CMatrix3DH
CQCamera3DNewCamera::
perspectiveMatrix() const
{
  updateOrientation();

  return perspectiveMatrix_;
}

void
CQCamera3DNewCamera::
calcPerspectiveMatrix()
{
  perspectiveMatrix_ = CMatrix3DH::perspective(fov(), aspect(), near(), far());
}

CMatrix3DH
CQCamera3DNewCamera::
orthoMatrix() const
{
  updateOrientation();

  return orthoMatrix_;
}

void
CQCamera3DNewCamera::
calcOrthoMatrix()
{
  orthoMatrix_ = CMatrix3DH::ortho(-1, 1, -1, 1, near(), far());
}

CMatrix3DH
CQCamera3DNewCamera::
viewMatrix() const
{
  updateOrientation();

  return viewMatrix_;
}

void
CQCamera3DNewCamera::
calcViewMatrix()
{
  //lookAt(front_, up_);

  //---

  auto position = origin_ - front_*distance_;

//auto tx = -position.x(), ty = -position.y(), tz = -position.z();

  //---

#if 0
  auto qx = orientation_.getX();
  auto qy = orientation_.getY();
  auto qz = orientation_.getZ();
  auto qw = orientation_.getW();

  auto x2 = qx + qx;
  auto y2 = qy + qy;
  auto z2 = qz + qz;

  auto x2x = x2*qx;
  auto x2y = x2*qy;
  auto x2z = x2*qz;
  auto x2w = x2*qw;
  auto y2y = y2*qy;
  auto y2z = y2*qz;
  auto y2w = y2*qw;
  auto z2z = z2*qz;
  auto z2w = z2*qw;

  double m00 = 1.0 - (y2y + z2z);
  double m10 = x2y - z2w;
  double m20 = x2z + y2w;

  double m01 = x2y + z2w;
  double m11 = 1.0 - (x2x + z2z);
  double m21 = y2z - x2w;

  double m02 = x2z - y2w;
  double m12 = y2z + x2w;
  double m22 = 1.0 - (x2x + y2y);

  viewMatrix_ = CMatrix3DH(m00, m01, m02,
                           m10, m11, m12,
                           m20, m21, m22,
                           tx, ty, tz);

  //std::cerr << viewMatrix_ << "\n\n";
#endif

#if 0
  orientation_.toRotationMatrix(viewMatrix_);

  viewMatrix_.translate(tx, ty, tz);
#endif

#if 0
  CMatrix3DH m1;
  orientation_.toRotationMatrix(m1);

  auto m2 = CMatrix3DH::translation(tx, ty, tz);

  viewMatrix_ = m2*m1;
#endif

  viewMatrix_.setLookAt(position.point(), front_, up_, right_);
}

//---

void
CQCamera3DNewCamera::
moveAroundX(double d)
{
  rotateX(0.03*d);
}

void
CQCamera3DNewCamera::
moveAroundY(double d)
{
  rotateY(-0.03*d);
}

void
CQCamera3DNewCamera::
moveAroundZ(double d)
{
  rotateZ(0.03*d);
}

//---

void
CQCamera3DNewCamera::
printMatrices() const
{
  auto position = origin_ - front_*distance_;

  auto tx = -position.x(), ty = -position.y(), tz = -position.z();

  //---

  auto qx = orientation_.getX();
  auto qy = orientation_.getY();
  auto qz = orientation_.getZ();
  auto qw = orientation_.getW();

  auto x2 = qx + qx;
  auto y2 = qy + qy;
  auto z2 = qz + qz;

  auto x2x = x2*qx;
  auto x2y = x2*qy;
  auto x2z = x2*qz;
  auto x2w = x2*qw;
  auto y2y = y2*qy;
  auto y2z = y2*qz;
  auto y2w = y2*qw;
  auto z2z = z2*qz;
  auto z2w = z2*qw;

  double m00 = 1.0 - (y2y + z2z);
  double m10 = x2y - z2w;
  double m20 = x2z + y2w;

  double m01 = x2y + z2w;
  double m11 = 1.0 - (x2x + z2z);
  double m21 = y2z - x2w;

  double m02 = x2z - y2w;
  double m12 = y2z + x2w;
  double m22 = 1.0 - (x2x + y2y);

  auto m1 = CMatrix3DH(m00, m01, m02,
                       m10, m11, m12,
                       m20, m21, m22,
                       tx, ty, tz);

  std::cerr << "Matrix 1: " << m1 << "\n";

  //---

  CMatrix3DH m2;
  orientation_.toRotationMatrix(m2);

  m2.translate(tx, ty, tz);

  std::cerr << "Matrix 2: " << m2 << "\n";

  //---

  std::cerr << "Orientation: " << orientation_ << "\n";
  std::cerr << "Look At: " << calcLookAt(front_, up_) << "\n";

  //---

  CMatrix3DH m3;
  orientation_.toRotationMatrix(m3);

  std::cerr << "Matrix 3: " << m3 << "\n";

  auto mt = CMatrix3DH::translation(tx, ty, tz);

  auto m4 = mt*m3;

  std::cerr << "Matrix 4: " << m4 << "\n";

  //---

  CMatrix3DH m5;
  m5.setLookAt(position.point(), front_, up_, right_);
  std::cerr << "Matrix 5: " << m5 << "\n";
}

CVector3D
CQCamera3DNewCamera::
toEuler(const CQuaternion &q) const
{
  auto xx = q.getX();
  auto yy = q.getY();
  auto zz = q.getZ();
  auto ww = q.getW();

  auto xsq = xx*xx;
  auto ysq = yy*yy;
  auto zsq = zz*zz;

  return CVector3D( std::atan2(2.0*(xx*ww - yy*zz), 1.0f - 2.0*(xsq + zsq)),
                    std::atan2(2.0*(yy*ww + xx*zz), 1.0f - 2.0*(ysq + zsq)),
                   -std::asin (2.0*(xx*yy + zz*ww)));
}

void
CQCamera3DNewCamera::
lookAt(const CVector3D &forward, const CVector3D &up)
{
  orientation_ = calcLookAt(forward, up);
}

CQuaternion
CQCamera3DNewCamera::
calcLookAt(const CVector3D &forward, const CVector3D &up) const
{
  // Based on typical vector to matrix to quaternion approach

  // Get orthogonal basis vectors
  auto right = up.crossProduct(forward).normalized();
  auto newUp = forward.crossProduct(right);

  double m0 = right.x()  ; double m1 = right.y()  ; double m2  = right.z()  ;
  double m4 = newUp.x()  ; double m5 = newUp.y()  ; double m6  = newUp.z()  ;
  double m8 = forward.x(); double m9 = forward.y(); double m10 = forward.z();

  double trace = m0 + m5 + m10;

  double ow, ox, oy, oz;

  if (trace > 0.0) {
    auto s = 0.5/std::sqrt(trace + 1.0);

    ow = 0.25/s;
    ox = (m6 - m9)*s;
    oy = (m8 - m2)*s;
    oz = (m1 - m4)*s;
  }
  else {
    if      (m0 > m5 && m0 > m10) {
      auto s = 2.0*std::sqrt(1.0 + m0 - m5 - m10);

      ow = (m6 - m9)/s;
      ox = 0.25*s;
      oy = (m4 + m1)/s;
      oz = (m8 + m2)/s;
    }
    else if (m5 > m10) {
      auto s = 2.0*std::sqrt(1.0 + m5 - m0 - m10);

      ow = (m8 - m2)/s;
      ox = (m4 + m1)/s;
      oy = 0.25*s;
      oz = (m9 + m6)/s;
    }
    else {
      auto s = 2.0*std::sqrt(1.0 + m10 - m0 - m5);

      ow = (m1 - m4)/s;
      ox = (m8 + m2)/s;
      oy = (m9 + m6)/s;
      oz = 0.25*s;
    }
  }

  return CQuaternion(ow, CVector3D(ox, oy, oz));
}

//---

void
CQCamera3DNewCamera::
getPixelRay(double x, double y, CPoint3D &rp1, CPoint3D &rp2) const
{
  updateOrientation();

  auto iperspectiveMatrix = perspectiveMatrix_.inverse();
  auto iviewMatrix        = viewMatrix_       .inverse();

  // unproject pixel point
  auto p1 = CVector3D(x, y, 0.0);
  auto p2 = iperspectiveMatrix*p1;

  auto rd = p2.unit();

  rp1 = (iviewMatrix* p2      ).point();
  rp2 = (iviewMatrix*(p2 + rd)).point();
}
