#include <CQCamera3DCamera.h>

#include <CQuaternion.h>

CQCamera3DCamera::
CQCamera3DCamera()
{
}

CQCamera3DCamera::
~CQCamera3DCamera()
{
}

void
CQCamera3DCamera::
setFov(double r)
{
  if (r != fov_) { // check for change
    CGLCameraIFace::setFov(r);

    Q_EMIT stateChanged();
  }
}

void
CQCamera3DCamera::
setAspect(double r)
{
  if (r != aspect_) { // check for change
    CGLCameraIFace::setAspect(r);

    Q_EMIT stateChanged();
  }
}

void
CQCamera3DCamera::
setNear(double r)
{
  if (r != near_) { // check for change
    CGLCameraIFace::setNear(r);

    Q_EMIT stateChanged();
  }
}

void
CQCamera3DCamera::
setFar(double r)
{
  if (r != far_) { // check for change
    CGLCameraIFace::setFar(r);

    Q_EMIT stateChanged();
  }
}

//---

void
CQCamera3DCamera::
setOrigin(const CVector3D &p)
{
  CGLCameraIFace::setOrigin(p);

  calcAngles();

  matrixValid_ = false;

  Q_EMIT stateChanged();
}

void
CQCamera3DCamera::
setPosition(const CVector3D &p)
{
  CGLCameraIFace::setPosition(p);

  matrixValid_ = false;

  Q_EMIT stateChanged();
}

//---

#if 0
void
CQCamera3DCamera::
setOriginXY(const CVector3D &p)
{
  setOrigin(p);
}

void
CQCamera3DCamera::
setOriginYZ(const CVector3D &p)
{
  setOrigin(p);
}

void
CQCamera3DCamera::
setOriginXZ(const CVector3D &p)
{
  setOrigin(p);
}
#endif

//---

void
CQCamera3DCamera::
setPitch(double r)
{
  if (r != pitch_) {
    CGLCameraIFace::setPitch(r);

    matrixValid_ = false;

    Q_EMIT stateChanged();
  }
}

void
CQCamera3DCamera::
setYaw(double r)
{
  if (r != yaw_) {
    CGLCameraIFace::setYaw(r);

    matrixValid_ = false;

    Q_EMIT stateChanged();
  }
}

void
CQCamera3DCamera::
setRoll(double r)
{
  if (r != roll_) {
    CGLCameraIFace::setRoll(r);

    matrixValid_ = false;

    Q_EMIT stateChanged();
  }
}

//---

// rotate around origin in x axis (pitch) - ZY
void
CQCamera3DCamera::
moveAroundX(double d)
{
  // calc z/y circle from origin (center) to position (edge)
  auto pov = CVector3D(origin_, position_);
  auto r   = pov.length();

  // calc delta angle to move distance along circumference
  auto circ = 2*M_PI*r; if (d >= circ) return; double da = M_PI*d/circ;
  auto m = CMatrix3DH::rotation(-da, right_); // rotate around right vector
  auto pov1 = m*pov;

  // update position
  position_ = pov1 + origin_;

  pitch_ -= da;

  auto cosY = std::cos(-yaw_ ); auto sinY = std::sin(-yaw_ );
  auto cosP = std::cos(pitch_); auto sinP = std::sin(pitch_);

  auto front = CVector3D(cosY*cosP, sinP, sinY*cosP);
  front_ = front.normalized();
  up_ = -front_.crossProduct(right_);

  //auto pitch1 = std::asin(front_.y());
  //std::cerr << "Pitch: " << pitch_ << " " << pitch1 << "\n";

  rotationMatrix_ = calcRotationMatrix();

  Q_EMIT stateChanged();
}

// rotate around origin in y axis (yaw) - XZ
void
CQCamera3DCamera::
moveAroundY(double d)
{
  // calc x/z circle from origin (center) to position (edge)
  auto pov = CVector3D(origin_, position_);
  auto r   = pov.length();

  // calc delta angle to move distance along circumference
  auto circ = 2*M_PI*r;
  if (d >= circ) return;

  double da = M_PI*d/circ;

  auto m = CMatrix3DH::rotation(da, up_); // rotate around up vector
  auto pov1 = m*pov;

  // update position
  position_ = pov1 + origin_;

  yaw_ += da;

  auto cosY = std::cos(-yaw_ ); auto sinY = std::sin(-yaw_ );
  auto cosP = std::cos(pitch_); auto sinP = std::sin(pitch_);

  auto front = CVector3D(cosY*cosP, sinP, sinY*cosP);
  front_ = front.normalized();
  right_ = front_.crossProduct(up_);

  //auto yaw1 = std::atan2(-front_.z(), front_.x());
  //std::cerr << "Yaw: " << yaw_ << " " << yaw1 << "\n";

  rotationMatrix_ = calcRotationMatrix();

  Q_EMIT stateChanged();
}

// rotate around origin in z axis (roll) - XY
void
CQCamera3DCamera::
moveAroundZ(double d)
{
  // calc x/y circle from origin (center) to position (edge)
  auto pov = CVector3D(origin_, position_);
  auto r   = pov.length();

  // calc delta angle to move distance along circumference
  auto circ = 2*M_PI*r;
  if (d >= circ) return;

  double da = M_PI*d/circ;

  auto m = CMatrix3DH::rotation(-da, front_); // rotate around front vector

  auto right1 = m*right_;
  auto up1    = m*up_;

  right_ = right1.normalized();
  up_    = up1   .normalized();

  // update roll_
  roll_ -= da;

  rotationMatrix_ = calcRotationMatrix();

  Q_EMIT stateChanged();
}

CVector3D
CQCamera3DCamera::
front() const
{
  if (! matrixValid_)
    const_cast<CQCamera3DCamera *>(this)->calcVectors();

  return front_;
}

CVector3D
CQCamera3DCamera::
up() const
{
  if (! matrixValid_)
    const_cast<CQCamera3DCamera *>(this)->calcVectors();

  return up_;
}

CVector3D
CQCamera3DCamera::
right() const
{
  if (! matrixValid_)
    const_cast<CQCamera3DCamera *>(this)->calcVectors();

  return right_;
}

//---

// get perspective matrix
CMatrix3DH
CQCamera3DCamera::
perspectiveMatrix() const
{
  return CMatrix3DH::perspective(fov(), aspect(), near(), far());
}

// get ortho matrix
CMatrix3DH
CQCamera3DCamera::
orthoMatrix() const
{
  auto pov = CVector3D(origin_, position_);
  auto r   = pov.length()/2.0;

  return CMatrix3DH::ortho(/*left*/-r, /*right*/r, /*bottom*/-r, /*top*/r, near(), far());
}

// get view matrix
CMatrix3DH
CQCamera3DCamera::
viewMatrix() const
{
  if (! matrixValid_)
    const_cast<CQCamera3DCamera *>(this)->calcVectors();

#if 1
  auto m2 = CMatrix3DH::translation(-position_.getX(), -position_.getY(), -position_.getZ());
//  auto m3 = CMatrix3DH::translation( position_.getX(),  position_.getY(),  position_.getZ());

#if 0
  //auto mi = rotationMatrix_.inverse();
  auto mt = rotationMatrix_.transposed();
  //if (mi == mt) std::cerr << "Equal\n";

  //return (rotationMatrix_*m2).inverse();
  return m2*mt;
#else
  CMatrix3DH m;
  m.setLookAt(position_.point(), front_, up_, right_);

  return m;
#endif
#else
  auto mp1 = CMatrix3DH::translation( position_.getX(),  position_.getY(),  position_.getZ());
  auto mp2 = CMatrix3DH::translation(-position_.getX(), -position_.getY(), -position_.getZ());
  auto mo1 = CMatrix3DH::translation( origin_  .getX(),  origin_  .getY(),  origin_  .getZ());
  auto mo2 = CMatrix3DH::translation(-origin_  .getX(), -origin_  .getY(), -origin_  .getZ());

  return mp2*mo1*rotationMatrix_*mo2;
#endif
}

// get view matrix
CMatrix3DH
CQCamera3DCamera::
viewRotationMatrix() const
{
  if (! matrixValid_)
    const_cast<CQCamera3DCamera *>(this)->calcVectors();

  auto position = viewRotationPosition();

  auto m2 = CMatrix3DH::translation(position.getX(), position.getY(), position.getZ());

  return m2*rotationMatrix_;
}

CMatrix3DH
CQCamera3DCamera::
calcRotationMatrix() const
{
  auto xv = CVector3D(1.0, 0.0, 0.0); // pitch
  auto yv = CVector3D(0.0, 1.0, 0.0); // yaw
  auto zv = CVector3D(0.0, 0.0, 1.0); // roll

#if 1
  auto qp = CQuaternion::angleAxis(pitch_, xv); // rotate around x axis
  auto qy = CQuaternion::angleAxis(yaw_  , yv); // rotate around y axis
  auto qr = CQuaternion::angleAxis(roll_ , zv); // rotate around z axis

  auto qryp = qy*qp*qr;
  qryp.normalize();

  CMatrix3D mryp;
  qryp.toRotationMatrix(mryp);

  return CMatrix3DH(mryp);
#else
  auto mp = CMatrix3DH::rotation(pitch_, xv ); // rotate around x axis
  auto yv1 = mp*yv;
  auto my = CMatrix3DH::rotation(yaw_  , yv1); // rotate around y axis
  auto zv1 = my*mp*zv;
  auto mr = CMatrix3DH::rotation(roll_ , zv1); // rotate around z axis

  return mr*my*mp;
#endif
}

CVector3D
CQCamera3DCamera::
viewRotationPosition() const
{
  return CVector3D(0, 0, -10); // for 2d overlay. TODO: customize
}

#if 0
CVector3D
CQCamera3DCamera::
unrollPoint(const CVector3D &p) const
{
  auto q = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

  q.normalize();

  CMatrix3D m;
  q.toRotationMatrix(m);

  return m.inverse()*p;
}

CVector3D
CQCamera3DCamera::
rollPoint(const CVector3D &p) const
{
  auto q = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

  q.normalize();

  CMatrix3D m;
  q.toRotationMatrix(m);

  return m*p;
}

CVector3D
CQCamera3DCamera::
unrollYawPoint(const CVector3D &p) const
{
  auto qy = CQuaternion::angleAxis(yaw_ , CVector3D(0.0, 1.0, 0.0)); // rotate around y axis
  auto qr = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

  auto q = qy*qr;
  q.normalize();

  CMatrix3D m;
  q.toRotationMatrix(m);

  return m.inverse()*p;
}

CVector3D
CQCamera3DCamera::
rollYawPoint(const CVector3D &p) const
{
  auto qy = CQuaternion::angleAxis(yaw_ , CVector3D(0.0, 1.0, 0.0)); // rotate around y axis
  auto qr = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

  auto q = qy*qr;
  q.normalize();

  CMatrix3D m;
  q.toRotationMatrix(m);

  return m*p;
}

CVector3D
CQCamera3DCamera::
unrollYawPitchPoint(const CVector3D &p) const
{
  auto qp = CQuaternion::angleAxis(pitch_, CVector3D(1.0, 0.0, 0.0)); // rotate around x axis
  auto qy = CQuaternion::angleAxis(yaw_  , CVector3D(0.0, 1.0, 0.0)); // rotate around y axis
  auto qr = CQuaternion::angleAxis(roll_ , CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

  auto q = qp*qy*qr;
  q.normalize();

  CMatrix3D m;
  q.toRotationMatrix(m);

  return m.inverse()*p;
}
#endif

// given euler angles calc front/up/right vectors
void
CQCamera3DCamera::
calcVectors()
{
  auto pov = CVector3D(position_, origin_);
  auto l = pov.length();

  rotationMatrix_ = calcRotationMatrix();

#if 0
  rotationMatrix_.getColumn(0, front_);
  rotationMatrix_.getColumn(1, up_   );
  rotationMatrix_.getColumn(2, right_);
#endif

#if 0
  auto front0 = rotationMatrix_*CVector3D(1, 0, 0);
  auto up0    = rotationMatrix_*CVector3D(0, 1, 0);
  auto right0 = rotationMatrix_*CVector3D(0, 0, 1);
  std::cerr << "Front: " << front0 << "\n";
  std::cerr << "Up: " << up0 << "\n";
  std::cerr << "Right: " << right0 << "\n";
#endif

#if 1
  auto cosY = std::cos(-yaw_ ); auto sinY = std::sin(-yaw_ );
  auto cosP = std::cos(pitch_); auto sinP = std::sin(pitch_);
  auto cosR = std::cos(roll_ ); auto sinR = std::sin(roll_ );

  auto front = CVector3D(cosY*cosP, sinP, sinY*cosP);
  //front.setZ(-front.z());
  auto up    = CVector3D(-cosY*sinR - sinY*sinP*cosR, cosP*cosR, -sinY*sinR - sinP*cosR*-cosY);
  auto right = front.crossProduct(up);

  //auto upr = -front.crossProduct(right);

  auto front1 = front.normalized();
  auto up1    = up   .normalized();
  auto right1 = right.normalized();

#if 0
  std::cerr << "Front: " << front_ << " " << front1 << "\n";
  std::cerr << "Up:    " << up_    << " " << up1    << "\n";
  std::cerr << "Right: " << right_ << " " << right1 << "\n";
#endif

  front_ = front1;
  up_    = up1;
  right_ = right1;
#endif

  origin_ = position_ + l*front_;

  matrixValid_ = true;
}

// given position and origin calc euler angles
void
CQCamera3DCamera::
calcAngles()
{
  // calc front
  auto front = CVector3D(position_, origin_);
  front.normalize();

  pitch_ = std::asin(front.y());               // inverse for front y value in calcVectors
  yaw_   = std::atan2(-front.z(), front.x());
//  roll_  = 0;

  calcVectors();
}
