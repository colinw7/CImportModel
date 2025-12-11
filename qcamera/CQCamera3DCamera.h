#ifndef CQCamera3DCamera_H
#define CQCamera3DCamera_H

#include <CQuaternion.h>
#include <CMatrix3DH.h>
#include <CVector3D.h>

#include <QObject>

#include <cmath>

/*
 * rotate by yaw, followed by pitch, followed by roll
 *
 * starting with vector (1, 0, 0), look down negative z axis is yaw=-90 degrees, pitch=0, roll=0
 */
class CQCamera3DCamera : public QObject {
  Q_OBJECT

 public:
  CQCamera3DCamera() { }

  //---

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //---

  // set perspective values

  // field of view
  double fov() const { return fov_; }
  void setFov(double r) { fov_ = r; Q_EMIT stateChanged(); }

  // pixel aspect
  double aspect() const { return aspect_; }
  void setAspect(double r) { aspect_ = r; Q_EMIT stateChanged(); }

  // near z
  double near() const { return near_; }
  void setNear(double r) { near_ = r; Q_EMIT stateChanged(); }

  // far z
  double far() const { return far_; }
  void setFar(double r) { far_ = r; Q_EMIT stateChanged(); }

  //---

  // origin (for rotation center)
  const CVector3D &origin() const { return origin_; }
  void setOrigin(const CVector3D &p) { origin_ = p; calcAngles(); Q_EMIT stateChanged(); }

  // position (for camera position)
  const CVector3D &position() const { return position_; }
  void setPosition(const CVector3D &p) { position_ = p; Q_EMIT stateChanged(); }

  //---

  // rotation x angle
  double pitch() const { return pitch_; }
  void setPitch(double r) { pitch_ = r; matrixValid_ = false; Q_EMIT stateChanged(); }

  // rotation x angle
  double yaw() const { return yaw_; }
  void setYaw(double r) { yaw_ = r; matrixValid_ = false; Q_EMIT stateChanged(); }

  // rotation z angle
  double roll() const { return roll_; }
  void setRoll(double r) { roll_ = r; matrixValid_ = false; Q_EMIT stateChanged(); }

  //---

  void rotatePosition(int axis, double f=0.1) {
    if      (axis == 0) pitch_ += f;
    else if (axis == 1) yaw_   += f;
    else if (axis == 2) roll_  += f;
    else assert(false);

    matrixValid_ = false;

    Q_EMIT stateChanged();
  }

  void moveOrigin(int axis, double f=0.1) {
    if      (axis == 0) origin_ += CVector3D(f, 0, 0);
    else if (axis == 1) origin_ += CVector3D(0, f, 0);
    else if (axis == 2) origin_ += CVector3D(0, 0, f);
    else assert(false);

    calcAngles();

    Q_EMIT stateChanged();
  }

  void movePosition(int axis, double f=0.1) {
    if      (axis == 0) position_ += CVector3D(f, 0, 0);
    else if (axis == 1) position_ += CVector3D(0, f, 0);
    else if (axis == 2) position_ += CVector3D(0, 0, f);
    else assert(false);

    Q_EMIT stateChanged();
  }

  void moveFront(double d) {
    setPosition(position_ + d*front());
  }

  void moveRight(double d) {
    setPosition(position_ + d*right());
  }

  void moveUp(double d) {
    setPosition(position_ + d*up());
  }

  // rotate around origin in x axis (pitch) - ZY
  void moveAroundX(double d) {
    // calc z/y circle from origin (center) to position (edge)
    auto pov = CVector3D(origin_, position_);
    auto r   = pov.length();

    // calc delta angle to move distance along circumference
    auto circ = 2*M_PI*r;
    if (d >= circ) return;

    double da = M_PI*d/circ;

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
  void moveAroundY(double d) {
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
  void moveAroundZ(double d) {
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

  void zoom(double dl) {
#if 0
    auto d = position_ - origin_;
    auto l = d.length();
    if (l <= 0.0) return;

    d *= 1.0/l;

    l -= dl;
    if (l <= 0.0) return;

    setPosition(origin_ + d*l);
#else
    auto d = front();

    setPosition(position_ + d*dl);
#endif
  }

  CVector3D front() const {
    if (! matrixValid_)
      const_cast<CQCamera3DCamera *>(this)->calcVectors();

    return front_;
  }

  CVector3D up() const {
    if (! matrixValid_)
      const_cast<CQCamera3DCamera *>(this)->calcVectors();

    return up_;
  }

  CVector3D right() const {
    if (! matrixValid_)
      const_cast<CQCamera3DCamera *>(this)->calcVectors();

    return right_;
  }

  //---

  // get perspective matrix
  CMatrix3DH perspectiveMatrix() const {
    return CMatrix3DH::perspective(fov_, aspect_, near_, far_);
  }

  // get view matrix
  CMatrix3DH viewMatrix() const {
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
  CMatrix3DH viewRotationMatrix() const {
    if (! matrixValid_)
      const_cast<CQCamera3DCamera *>(this)->calcVectors();

    auto position = viewRotationPosition();

    auto m2 = CMatrix3DH::translation(position.getX(), position.getY(), position.getZ());

    return m2*rotationMatrix_;
  }

  CMatrix3DH calcRotationMatrix() const {
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

  CVector3D viewRotationPosition() const {
    return CVector3D(0, 0, -10); // for 2d overlay. TODO: customize
  }

#if 0
  CVector3D unrollPoint(const CVector3D &p) const {
    auto q = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

    q.normalize();

    CMatrix3D m;
    q.toRotationMatrix(m);

    return m.inverse()*p;
  }

  CVector3D rollPoint(const CVector3D &p) const {
    auto q = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

    q.normalize();

    CMatrix3D m;
    q.toRotationMatrix(m);

    return m*p;
  }

  CVector3D unrollYawPoint(const CVector3D &p) const {
    auto qy = CQuaternion::angleAxis(yaw_ , CVector3D(0.0, 1.0, 0.0)); // rotate around y axis
    auto qr = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

    auto q = qy*qr;
    q.normalize();

    CMatrix3D m;
    q.toRotationMatrix(m);

    return m.inverse()*p;
  }

  CVector3D rollYawPoint(const CVector3D &p) const {
    auto qy = CQuaternion::angleAxis(yaw_ , CVector3D(0.0, 1.0, 0.0)); // rotate around y axis
    auto qr = CQuaternion::angleAxis(roll_, CVector3D(0.0, 0.0, 1.0)); // rotate around z axis

    auto q = qy*qr;
    q.normalize();

    CMatrix3D m;
    q.toRotationMatrix(m);

    return m*p;
  }

  CVector3D unrollYawPitchPoint(const CVector3D &p) const {
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

 private:
  // given euler angles calc front/up/right vectors
  void calcVectors() {
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
  void calcAngles() {
    // calc front
    auto front = CVector3D(position_, origin_);
    front.normalize();

    pitch_ = std::asin(front.y());               // inverse for front y value in calcVectors
    yaw_   = std::atan2(-front.z(), front.x());
//  roll_  = 0;

    calcVectors();
  }

 Q_SIGNALS:
  // send when state (data values) changed
  void stateChanged();

 private:
  bool visible_ { true };

  double fov_    { 45.0 };      // field of view
  double aspect_ { 1.0 };       // pixel aspect
  double near_   { 1.0 };       // near z (too small get z fighting)
  double far_    { 1000000.0 }; // far z

  CVector3D origin_   { 0.0, 0.0, 0.0 }; // camera rotation center
  CVector3D position_ { 1.0, 1.0, 1.0 }; // camera position

  double pitch_ { 0.0 };   // x axis rotation
  double yaw_   { -M_PI }; // y axis rotation
  double roll_  { 0.0 };   // z axis rotation

  bool matrixValid_ { false };

  CMatrix3DH rotationMatrix_;

  CVector3D front_;
  CVector3D up_;
  CVector3D right_;
};

#endif
