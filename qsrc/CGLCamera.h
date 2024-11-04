#ifndef CGL_CAMERA_H
#define CGL_CAMERA_H

#include <CGLMatrix3D.h>
#include <CGLVector3D.h>

#include <QDateTime>

//#include <glad/glad.h>

#include <vector>

// An abstract camera class that processes input and calculates the corresponding Euler Angles,
// Vectors and Matrices for use in OpenGL
class CGLCamera {
 public:
  // Defines several possible options for camera movement. Used as abstraction to stay
  // away from window-system specific input methods
  enum class Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
  };

 public:
  static float s_pitch() { return PITCH; }
  static float s_yaw() { return YAW; }

 public:
  // constructor with vectors
  CGLCamera(const CGLVector3D &position = CGLVector3D(0.0f, 0.0f, 0.0f),
            const CGLVector3D &up       = CGLVector3D(0.0f, 1.0f, 0.0f),
            float yaw = YAW, float pitch = PITCH) {
    position_ = position;
    worldUp_  = up;
    yaw_      = yaw;
    pitch_    = pitch;

    init();
  }

  // constructor with scalar values
  CGLCamera(float posX, float posY, float posZ, float upX, float upY, float upZ,
            float yaw, float pitch) {
    position_ = CGLVector3D(posX, posY, posZ);
    worldUp_  = CGLVector3D(upX, upY, upZ);
    yaw_      = yaw;
    pitch_    = pitch;

    init();
  }

  void init() {
    startTime_ = QDateTime::currentDateTime();

    updateCameraVectors();
  }

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  CGLMatrix3D getViewMatrix() {
    return CGLMatrix3D::lookAt(position_, position_ + front_, up_);
  }

  const CGLVector3D &position() const { return position_; }

  float zoom() const { return zoom_; }

  // processes input received from any keyboard-like input system.
  // Accepts input parameter in the form of camera defined ENUM
  // (to abstract it from windowing systems)
  void processKeyboard(Movement direction, float deltaTime) {
    if      (direction == Movement::FORWARD)
      moveIn(deltaTime);
    else if (direction == Movement::BACKWARD)
      moveOut(deltaTime);
    else if (direction == Movement::LEFT)
      moveLeft(deltaTime);
    else if (direction == Movement::RIGHT)
      moveRight(deltaTime);
  }

  void moveLeft (float deltaTime=0.1) {
    float velocity = movementSpeed_*deltaTime; position_ -= right_*velocity; }
  void moveRight(float deltaTime=0.1) {
    float velocity = movementSpeed_*deltaTime; position_ += right_*velocity; }
  void moveUp   (float deltaTime=0.1) {
    float velocity = movementSpeed_*deltaTime; position_ -= up_   *velocity; }
  void moveDown (float deltaTime=0.1) {
    float velocity = movementSpeed_*deltaTime; position_ += up_   *velocity; }
  void moveIn   (float deltaTime=0.1) {
    float velocity = movementSpeed_*deltaTime; position_ += front_*velocity; }
  void moveOut  (float deltaTime=0.1) {
    float velocity = movementSpeed_*deltaTime; position_ -= front_*velocity; }

  // processes input received from a mouse input system. Expects the offset value in
  // both the x and y direction.
  void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
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

  // processes input received from a mouse scroll-wheel event. Only requires input on
  // the vertical wheel-axis
  void processMouseScroll(float yoffset) {
    zoom_ -= float(yoffset);

    if (zoom_ < 1.0f)
      zoom_ = 1.0f;
    if (zoom_ > 90.0f)
      zoom_ = 90.0f;
  }

  void setLastPos(float x, float y) {
    lastX_ = x;
    lastY_ = y;
  }

  void deltaPos(float x, float y, float &dx, float &dy) {
    dx  = x - lastX_;
    dy  = lastY_ - y; // reversed since y-coordinates go from bottom to top
  }

  bool isFirstPos() const { return firstPos_; }
  void setFirstPos(bool b) { firstPos_ = b; }

  void updateFrameTime() {
    //float currentFrame = glfwGetTime();
    float currentFrame = float(-QDateTime::currentDateTime().secsTo(startTime_));

    deltaTime_ = currentFrame - lastFrame_;
    lastFrame_ = currentFrame;
  }

  float deltaTime() const { return deltaTime_; }

  static float DegToRad(float d) {
    return float(M_PI*d/180.0);
  }

  float yaw() const { return yaw_; }
  void setYaw(double r) { yaw_ = r; updateCameraVectors(); }

  float pitch() const { return pitch_; }
  void setPitch(double r) { pitch_ = r; updateCameraVectors(); }

 private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors() {
    // calculate the new front_ vector
    CGLVector3D front(std::cos(DegToRad(yaw  ()))*std::cos(DegToRad(pitch())),
                      std::sin(DegToRad(pitch())),
                      std::sin(DegToRad(yaw  ()))*std::cos(DegToRad(pitch())));

    front_ = front.normalized();

    // also re-calculate the right_ and up_ vector
    // normalize the vectors, because their length gets closer to 0 the more you look up or down
    // which results in slower movement.
    right_ = front_.crossProduct(worldUp_).normalized();
    up_    = right_.crossProduct(front_  ).normalized();
  }

 private:
  // Default camera values
  static constexpr float YAW         = -90.0f;
  static constexpr float PITCH       =  0.0f;
  static constexpr float SPEED       =  2.5f;
  static constexpr float SENSITIVITY =  0.1f;
  static constexpr float ZOOM        =  45.0f;

  // camera Attributes
  CGLVector3D position_;
  CGLVector3D front_ { 0.0f, 0.0f, -1.0f };
  CGLVector3D up_;
  CGLVector3D right_;
  CGLVector3D worldUp_;

  // euler Angles
  float yaw_   { YAW };
  float pitch_ { PITCH };

  // camera options
  float movementSpeed_    { SPEED };
  float mouseSensitivity_ { SENSITIVITY };
  float zoom_             { ZOOM };

  // movement data
  float lastX_ { 0.0 };
  float lastY_ { 0.0 };

  bool firstPos_ { true };

  // timing
  QDateTime startTime_;

  float deltaTime_ { 0.0f }; // time between current frame and last frame
  float lastFrame_ { 0.0f };
};

#endif
