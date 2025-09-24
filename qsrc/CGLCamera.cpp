#include <CGLCamera.h>
#include <CPlane3D.h>

CGLCamera::
CGLCamera(const CGLVector3D &position, const CGLVector3D &up, float yaw, float pitch)
{
  init(position, up, yaw, pitch);
}

CGLCamera::
CGLCamera(float posX, float posY, float posZ, float upX, float upY, float upZ,
          float yaw, float pitch)
{
  init(CGLVector3D(posX, posY, posZ), CGLVector3D(upX, upY, upZ), yaw, pitch);
}

CGLCamera::
CGLCamera(const CGLVector3D &origin, const CGLVector3D &position, const CGLVector3D &up)
{
  init(origin, position, up);
}

void
CGLCamera::
init(const CGLVector3D &position, const CGLVector3D &up, float yaw, float pitch)
{
  origin_   = CGLVector3D(0, 0, 0);
  position_ = position;
  worldUp_  = up;

  yaw_   = yaw;
  pitch_ = pitch;
  roll_  = CMathGen::RadToDeg(std::atan2(up.y(), up.x()));

  init();

  auto l = position_.length();

  origin_ = position_ + l*front_;
}

void
CGLCamera::
init(const CGLVector3D &origin, const CGLVector3D &position, const CGLVector3D &up)
{
  origin_   = origin;
  position_ = position;
  worldUp_  = up;

  double yaw, pitch, roll;

#if 0
  float xangle, yangle, zangle;
  CGLMatrix3D::calcEye(origin.x(), origin.y(), origin.z(),
                       position.x(), position.y(), position.z(),
                       &xangle, &yangle, &zangle);
  yaw   = xangle;
  pitch = yangle;
  roll  = zangle;
#else
  auto v = origin_ - position_; // eye to center
  auto l = v.length();
#endif
#if 0
  pitch = (l > 0 ? std::asin(v.y()/l) : 0.0);

  auto cp = std::cos(pitch);

  yaw  = (cp != 0.0 && l > 0 ? std::asin(v.x()/(cp*l)) : 0.0);
  roll = std::atan2(up.y(), up.x());
#else
  auto dx = position.x() - origin.x();
//auto dy = position.y() - origin.y();
  auto dz = position.z() - origin.z();

  yaw   = std::atan2(l, dx);
  pitch = -std::atan2(l, -dz);
  roll  = M_PI/2.0;
#endif

  pitch_ = CMathGen::RadToDeg(pitch);
  yaw_   = CMathGen::RadToDeg(yaw);
  roll_  = CMathGen::RadToDeg(roll);

  init();
}

void
CGLCamera::
init()
{
  startTime_ = QDateTime::currentDateTime();

  updateCameraVectors(/*rotate*/false);
}

void
CGLCamera::
setOrigin(const CGLVector3D &p)
{
  origin_ = p;

  // keep position
  updateViewMatrix(/*rotate*/false);
}

void
CGLCamera::
setPosition(const CGLVector3D &p)
{
  position_ = p;

  // keep origin
  updateViewMatrix(/*rotate*/false);
}

void
CGLCamera::
setUp(const CGLVector3D &p)
{
  up_ = p;

  // keep origin and position
  updateViewMatrix(/*rotate*/false);
}

void
CGLCamera::
setRight(const CGLVector3D &p)
{
  right_ = p;

  // keep origin and position
  updateViewMatrix(/*rotate*/false);
}

void
CGLCamera::
setZoom(float z)
{
  zoom_ = z;
}

CGLMatrix3D
CGLCamera::
getPerspectiveMatrix(float aspect) const
{
  return CGLMatrix3D::perspective(zoom(), aspect, near(), far());
}

CGLMatrix3D
CGLCamera::
getOrthoMatrix() const
{
  return CGLMatrix3D::ortho(/*left*/-1, /*right*/1, /*bottom*/-1, /*top*/1, near(), far());
}

void
CGLCamera::
processKeyboard(Movement direction, const ProcessKeyData &data)
{
  if      (direction == Movement::FORWARD) {
    if (data.rotate)
      rotateForward(1);
    else
      moveIn(data.deltaTime, isStrafe(), CGLCamera::RotateAt::POSITION);
  }
  else if (direction == Movement::BACKWARD) {
    if (data.rotate)
      rotateBack(1);
    else
      moveOut(data.deltaTime, isStrafe(), CGLCamera::RotateAt::POSITION);
  }
  else if (direction == Movement::ROTATE_LEFT) {
    rotateLeft(1, data.rotateAt);
  }
  else if (direction == Movement::ROTATE_RIGHT) {
    rotateRight(1, data.rotateAt);
  }
  else if (direction == Movement::STRAFE_LEFT) {
    moveLeft(data.deltaTime, /*strafe*/true, CGLCamera::RotateAt::POSITION);
  }
  else if (direction == Movement::STRAFE_RIGHT) {
    moveRight(data.deltaTime, /*strafe*/true, CGLCamera::RotateAt::POSITION);
  }
  else if (direction == Movement::LEFT) {
    if (data.rotate)
      rotateLeft(1, CGLCamera::RotateAt::POSITION);
    else {
      if (data.strafe)
        moveLeft(data.deltaTime, /*strafe*/true, CGLCamera::RotateAt::POSITION);
      else
        rotateLeft(1, CGLCamera::RotateAt::POSITION);
    }
  }
  else if (direction == Movement::RIGHT) {
    if (data.rotate)
      rotateRight(1, CGLCamera::RotateAt::POSITION);
    else {
      if (data.strafe)
        moveRight(data.deltaTime, /*strafe*/true, CGLCamera::RotateAt::POSITION);
      else
        rotateRight(1, CGLCamera::RotateAt::POSITION);
    }
  }
  else if (direction == Movement::UP) {
    if (data.rotate)
      rotateUp(1);
    else
      moveUp(data.deltaTime, data.strafe, CGLCamera::RotateAt::POSITION);
  }
  else if (direction == Movement::DOWN) {
    if (data.rotate)
      rotateDown(1);
    else
      moveDown(data.deltaTime, data.strafe, CGLCamera::RotateAt::POSITION);
  }
}

void
CGLCamera::
moveLeft(float deltaTime, bool strafe, const CGLCamera::RotateAt &rotateAt)
{
  float velocity = movementSpeed_*deltaTime;

  if (strafe) {
    position_ -= right()*velocity;
    origin_   -= right()*velocity;
  }
  else {
    if (rotateAt == CGLCamera::RotateAt::POSITION) {
      auto l = origin_.getDistance(position_);

      position_ -= right()*velocity;

      updateCameraVectorsI(false);

      origin_ = position_ + front_*l;
    }
    else
      origin_ -= right()*velocity;
  }

  // keep origin
  updateCameraVectors(/*rotate*/false);
}

void
CGLCamera::
moveRight(float deltaTime, bool strafe, const CGLCamera::RotateAt &rotateAt)
{
  float velocity = movementSpeed_*deltaTime;

  if (strafe) {
    position_ += right()*velocity;
    origin_   += right()*velocity;
  }
  else {
    if (rotateAt == CGLCamera::RotateAt::POSITION) {
      auto l = origin_.getDistance(position_);

      position_ += right()*velocity;

      updateCameraVectorsI(false);

      origin_ = position_ + front_*l;
    }
    else
      origin_ += right()*velocity;
  }

  // keep origin
  updateCameraVectors(/*rotate*/false);
}

void
CGLCamera::
moveUp(float deltaTime, bool strafe, const CGLCamera::RotateAt &rotateAt)
{
  float velocity = movementSpeed_*deltaTime;

  if (strafe) {
    position_ -= up_*velocity;
    origin_   -= up_*velocity;
  }
  else {
    if (rotateAt == CGLCamera::RotateAt::POSITION) {
      auto l = origin_.getDistance(position_);

      position_ -= up_*velocity;

      updateCameraVectorsI(false);

      origin_ = position_ + front_*l;
    }
    else
      origin_ -= up_*velocity;
  }

  // keep origin
  updateViewMatrix(/*rotate*/false);
}

void
CGLCamera::
moveDown(float deltaTime, bool strafe, const CGLCamera::RotateAt &rotateAt)
{
  float velocity = movementSpeed_*deltaTime;

  if (strafe) {
    position_ += up_*velocity;
    origin_   += up_*velocity;
  }
  else {
    if (rotateAt == CGLCamera::RotateAt::POSITION) {
      auto l = origin_.getDistance(position_);

      position_ += up_*velocity;

      updateCameraVectorsI(false);

      origin_ = position_ + front_*l;
    }
    else
      origin_ += up_*velocity;
  }

  // keep origin
  updateViewMatrix(/*rotate*/false);
}

void
CGLCamera::
rotateLeft(float deltaAngle, const CGLCamera::RotateAt &rotateAt)
{
#if 0
  yaw_ += deltaAngle;

  updateCameraVectors(/*rotate*/true);
#else
  if (rotateAt == CGLCamera::RotateAt::POSITION) {
    auto m1 = CGLMatrix3D::translation(position_.x(), position_.y(), position_.z());
//  auto m2 = CGLMatrix3D::rotation(CMathGen::Y_AXIS_3D, CMathGen::DegToRad(deltaAngle));
    auto m2 = CGLMatrix3D::rotation(CMathGen::DegToRad(deltaAngle), up_);
    auto m3 = CGLMatrix3D::translation(-position_.x(), -position_.y(), -position_.z());

    auto o = m1*m2*m3*origin_;
    origin_ = o;
  }
  else {
    auto m1 = CGLMatrix3D::translation(origin_.x(), origin_.y(), origin_.z());
//  auto m2 = CGLMatrix3D::rotation(CMathGen::Y_AXIS_3D, CMathGen::DegToRad(deltaAngle));
    auto m2 = CGLMatrix3D::rotation(CMathGen::DegToRad(deltaAngle), up_);
    auto m3 = CGLMatrix3D::translation(-origin_.x(), -origin_.y(), -origin_.z());

    auto p = m1*m2*m3*position_;
    position_ = p;
  }

  updateCameraVectors(/*rotate*/false);
#endif
}

void
CGLCamera::
rotateRight(float deltaAngle, const CGLCamera::RotateAt &rotateAt)
{
#if 0
  yaw_ -= deltaAngle;

  updateCameraVectors(/*rotate*/true);
#else
  if (rotateAt == CGLCamera::RotateAt::POSITION) {
    auto m1 = CGLMatrix3D::translation(position_.x(), position_.y(), position_.z());
//  auto m2 = CGLMatrix3D::rotation(CMathGen::Y_AXIS_3D, CMathGen::DegToRad(-deltaAngle));
    auto m2 = CGLMatrix3D::rotation(CMathGen::DegToRad(-deltaAngle), up_);
    auto m3 = CGLMatrix3D::translation(-position_.x(), -position_.y(), -position_.z());

    auto o = m1*m2*m3*origin_;
    origin_ = o;
  }
  else {
    auto m1 = CGLMatrix3D::translation(origin_.x(), origin_.y(), origin_.z());
//  auto m2 = CGLMatrix3D::rotation(CMathGen::Y_AXIS_3D, CMathGen::DegToRad(-deltaAngle));
    auto m2 = CGLMatrix3D::rotation(CMathGen::DegToRad(-deltaAngle), up_);
    auto m3 = CGLMatrix3D::translation(-origin_.x(), -origin_.y(), -origin_.z());

    auto p = m1*m2*m3*position_;
    position_ = p;
  }

  updateCameraVectors(/*rotate*/false);
#endif
}

void
CGLCamera::
rotateUp(float deltaAngle)
{
  pitch_ += deltaAngle;

  updateCameraVectors(/*rotate*/true);
}

void
CGLCamera::
rotateDown(float deltaAngle)
{
  pitch_ -= deltaAngle;

  updateCameraVectors(/*rotate*/true);
}

void
CGLCamera::
rotateForward(float deltaAngle)
{
  roll_ += deltaAngle;

  updateRoll();
}

void
CGLCamera::
rotateBack(float deltaAngle)
{
  roll_ -= deltaAngle;

  updateRoll();
}

void
CGLCamera::
moveIn(float deltaTime, bool strafe, const CGLCamera::RotateAt &rotateAt)
{
  float velocity = movementSpeed_*deltaTime;

  if (strafe) {
    position_ += front_*velocity;
    origin_   += front_*velocity;
  }
  else {
    if (rotateAt == CGLCamera::RotateAt::POSITION)
      position_ += front_*velocity;
    else
      origin_ += front_*velocity;
  }

  // keep origin
  updateCameraVectors(/*rotate*/false);
}

void
CGLCamera::
moveOut(float deltaTime, bool strafe, const CGLCamera::RotateAt &rotateAt)
{
  float velocity = movementSpeed_*deltaTime;

  if (strafe) {
    position_ -= front_*velocity;
    origin_   -= front_*velocity;
  }
  else {
    if (rotateAt == CGLCamera::RotateAt::POSITION)
      position_ -= front_*velocity;
    else
      origin_ -= front_*velocity;
  }

  // keep origin
  updateCameraVectors(/*rotate*/false);
}

void
CGLCamera::
processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
  xoffset *= mouseSensitivity_;
  yoffset *= mouseSensitivity_;

  yaw_   += xoffset;
  pitch_ += yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (pitch_ > 89.0f)
      pitch_ = 89.0f;
    if (pitch_ < -89.0f)
      pitch_ = -89.0f;
  }

  // update front, right and up Vectors using the updated Euler angles
  updateCameraVectors(/*rotate*/true);
}

void
CGLCamera::
processMouseScroll(float yoffset)
{
  zoom_ -= float(yoffset);

  if (zoom_ < 1.0f)
    zoom_ = 1.0f;
  if (zoom_ > 90.0f)
    zoom_ = 90.0f;
}

void
CGLCamera::
setLastPos(float x, float y)
{
  lastX_ = x;
  lastY_ = y;
}

void
CGLCamera::
deltaPos(float x, float y, float &dx, float &dy)
{
  dx = x - lastX_;
  dy = lastY_ - y; // reversed since y-coordinates go from bottom to top
}

void
CGLCamera::
setFirstPos(bool b)
{
  firstPos_ = b;
}

void
CGLCamera::
updateFrameTime()
{
  //float currentFrame = glfwGetTime();
  float currentFrame = float(-QDateTime::currentDateTime().secsTo(startTime_));

  deltaTime_ = currentFrame - lastFrame_;
  lastFrame_ = currentFrame;
}

void
CGLCamera::
setYaw(float a)
{
  yaw_ = a;

  // keep world up
  updateCameraVectors(/*rotate*/true);
}

void
CGLCamera::
setPitch(float a)
{
  pitch_ = a;

  // keep world up
  updateCameraVectors(/*rotate*/true);
}

void
CGLCamera::
setRoll(float a)
{
  roll_ = a;

  // re-calc world up and update
  updateRoll();
}

void
CGLCamera::
updateRoll()
{
  auto roll1 = CMathGen::DegToRad(roll());

  auto cosR = std::cos(roll1);
  auto sinR = std::sin(roll1);

  worldUp_ = CGLVector3D(cosR, sinR, 0.0);

  updateCameraVectors(/*rotate*/false);
}

void
CGLCamera::
updateCameraVectors(bool rotate)
{
  updateCameraVectorsI(rotate);

  updateViewMatrix(rotate);
}

void
CGLCamera::
updateCameraVectorsI(bool rotate)
{
  CGLVector3D front;

  // calculate the new front vector
  if (rotate) {
    auto pitch1 = CMathGen::DegToRad(pitch());
    auto yaw1   = CMathGen::DegToRad(yaw());
//  auto roll1  = CMathGen::DegToRad(roll());

    auto cosY = std::cos(yaw1  ); auto sinY = std::sin(yaw1  );
    auto cosP = std::cos(pitch1); auto sinP = std::sin(pitch1);
//  auto cosR = std::cos(roll1 ); auto sinR = std::sin(roll1 );

    front = CGLVector3D(cosY*cosP, sinP, sinY*cosP);
  //front = CGLVector3D(sinY*cosP, sinP, cosP*-cosY);

//  worldUp_ = CGLVector3D(-cosY*sinR - sinY*sinP*cosR, cosP*cosR, -sinY*sinR - sinP*cosR*-cosY);
  }
  else {
    if (position_ == origin_)
      position_ = origin_ + CGLVector3D(1, 1, 1);

    //front = CGLVector3D(origin_, position_); // position_ - origin_
    front = CGLVector3D(position_, origin_); // origin_ - position
  }

  front_ = front.normalized();

  // also re-calculate the right and up vector
  // normalize the vectors, because their length gets closer to 0 the more you look up or down
  // which results in slower movement.
  right_ = front_.crossProduct(worldUp_).normalized();
  up_    = right_.crossProduct(front_  ).normalized();
}

void
CGLCamera::
updateViewMatrix(bool rotate)
{
  // rotate about origin
  if (rotate) {
    auto l = origin_.getDistance(position_);

    if (rotateAt_ == RotateAt::ORIGIN)
      position_ = origin_ - front_*l;
    else
      origin_ = position_ + front_*l;
  }

  // eye, center, up
  viewMatrix_ = CGLMatrix3D::lookAt(position_, origin_, up_);
//viewMatrix_ = CGLMatrix3D::lookAt(origin_, position_, up_);
//viewMatrix_.setLookAt(position_.point(), origin_.point(), up_);

  viewChanged();
}

CQuaternion
CGLCamera::
trackBall(CVector3D &cop, CVector3D &cor, CVector3D &dir1, CVector3D &dir2) const
{
  // Implement track ball functionality to spin stuff on the screen
  //  cop   center of projection
  //  cor   center of rotation
  //  dir1  old mouse direction
  //  dir2  new mouse direction
  // pretend there is a sphere around cor.  Then find the points where dir1 and dir2
  // intersect that sphere. Find the rotation that takes the first point to the second.
  // If the line "dir" doesn't hit the sphere then take the closest point on the sphere
  // to that line.
  double m;

  // compute plane

  CVector3D nrml = cor - cop;

  // since trackball proportional to distance from cop
  double fudgefactor = 1.0;

  double len = nrml.length();

  if (len > 0)
    fudgefactor = 1.0/(len * 0.25);

  nrml.normalize();

  double dist = -nrml.dotProduct(cor);

  CPlane3D plane(nrml, dist);

  CPoint3D u;
  double   uparam = 0.0;

  CLine3D line1(cop.point(), (cop + dir1).point());

  plane.intersect(line1, u, uparam);

  CVector3D uv(u);

  uv = uv - cor;
  uv = uv * fudgefactor;
  m  = uv.length();

  if (m > 1.0) {
    uv = uv*1.0/m;
  }
  else {
    uv = uv - (nrml*sqrt(1 - m*m));
  }

  CPoint3D v;
  double   vparam = 0.0;

  CLine3D line2(cop.point(), (cop + dir2).point());

  plane.intersect(line2, v, vparam);

  CVector3D vv(v);

  vv = vv - cor;
  vv = vv * fudgefactor;
  m  = CVector3D(vv).length();

  if (m > 1.0) {
    vv = vv*1.0/m;
  }
  else {
    vv = vv - (nrml*sqrt(1 - m*m));
  }

  return CQuaternion::rotationArc(uv, vv);
}
