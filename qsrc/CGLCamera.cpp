#include <CGLCamera.h>

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

void
CGLCamera::
init(const CGLVector3D &position, const CGLVector3D &up, float yaw, float pitch)
{
  position_ = position;
  worldUp_  = up;

  yaw_   = yaw;
  pitch_ = pitch;
  roll_  = CMathGen::RadToDeg(std::atan2(up.y(), up.x()));

  init();
}

void
CGLCamera::
init()
{
  startTime_ = QDateTime::currentDateTime();

  updateCameraVectors();
}

void
CGLCamera::
setPosition(const CGLVector3D &p)
{
  position_ = p;

  updateViewMatrix();
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

void
CGLCamera::
processKeyboard(Movement direction, float deltaTime)
{
  if      (direction == Movement::FORWARD)
    moveIn(deltaTime);
  else if (direction == Movement::BACKWARD)
    moveOut(deltaTime);
  else if (direction == Movement::LEFT) {
    if (isRotate())
      rotateLeft(1);
    else
      moveLeft(deltaTime);
  }
  else if (direction == Movement::RIGHT) {
    if (isRotate())
      rotateRight(1);
    else
      moveRight(deltaTime);
  }
  else if (direction == Movement::UP) {
    if (isRotate())
      rotateUp(1);
    else
      moveUp(deltaTime);
  }
  else if (direction == Movement::DOWN) {
    if (isRotate())
      rotateDown(1);
    else
      moveDown(deltaTime);
  }
}

void
CGLCamera::
moveLeft(float deltaTime)
{
  float velocity = movementSpeed_*deltaTime;

  position_ -= right_*velocity;

  updateViewMatrix();
}

void
CGLCamera::
moveRight(float deltaTime)
{
  float velocity = movementSpeed_*deltaTime;

  position_ += right_*velocity;

  updateViewMatrix();
}

void
CGLCamera::
moveUp(float deltaTime)
{
  float velocity = movementSpeed_*deltaTime;

  position_ -= up_*velocity;

  updateViewMatrix();
}

void
CGLCamera::
moveDown(float deltaTime)
{
  float velocity = movementSpeed_*deltaTime;

  position_ += up_*velocity;

  updateViewMatrix();
}

void
CGLCamera::
rotateLeft(float deltaAngle)
{
  yaw_ += deltaAngle;

  updateCameraVectors(/*rotate*/true);
}

void
CGLCamera::
rotateRight(float deltaAngle)
{
  yaw_ -= deltaAngle;

  updateCameraVectors(/*rotate*/true);
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
moveIn(float deltaTime)
{
  float velocity = movementSpeed_*deltaTime;

  position_ += front_*velocity;

  updateViewMatrix();
}

void
CGLCamera::
moveOut(float deltaTime)
{
  float velocity = movementSpeed_*deltaTime;

  position_ -= front_*velocity;

  updateViewMatrix();
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

  // update front_, right_ and up_ Vectors using the updated Euler angles
  updateCameraVectors();
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

  updateCameraVectors();
}

void
CGLCamera::
setPitch(float a)
{
  pitch_ = a;

  updateCameraVectors();
}

void
CGLCamera::
setRoll(float a)
{
  worldUp_ = CGLVector3D(std::cos(CMathGen::DegToRad(a)),
                         std::sin(CMathGen::DegToRad(a)),
                         0.0);

  updateCameraVectors();
}

void
CGLCamera::
updateCameraVectors(bool rotate)
{
  // calculate the new front_ vector
  CGLVector3D front(std::cos(CMathGen::DegToRad(yaw  ()))*std::cos(CMathGen::DegToRad(pitch())),
                    std::sin(CMathGen::DegToRad(pitch())),
                    std::sin(CMathGen::DegToRad(yaw  ()))*std::cos(CMathGen::DegToRad(pitch())));

  front_ = front.normalized();

  // also re-calculate the right_ and up_ vector
  // normalize the vectors, because their length gets closer to 0 the more you look up or down
  // which results in slower movement.
  right_ = front_.crossProduct(worldUp_).normalized();
  up_    = right_.crossProduct(front_  ).normalized();

  updateViewMatrix(rotate);
}

void
CGLCamera::
updateViewMatrix(bool rotate)
{
  if (rotate) {
    auto l = origin_.getDistance(position_);

    position_ = origin_ - front_*l;
  }

  viewMatrix_ = CGLMatrix3D::lookAt(position_, position_ + front_, up_);
}
