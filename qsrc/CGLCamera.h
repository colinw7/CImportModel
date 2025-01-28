#ifndef CGL_CAMERA_H
#define CGL_CAMERA_H

#include <CGLMatrix3D.h>
#include <CGLVector3D.h>

#include <QDateTime>

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
    RIGHT,
    UP,
    DOWN
  };

 public:
  static float s_pitch() { return PITCH; }
  static float s_yaw() { return YAW; }

 public:
  // constructor with vectors
  CGLCamera(const CGLVector3D &position = CGLVector3D(0.0f, 0.0f, 0.0f),
            const CGLVector3D &up       = CGLVector3D(0.0f, 1.0f, 0.0f),
            float yaw = YAW, float pitch = PITCH);

  // constructor with scalar values
  CGLCamera(float posX, float posY, float posZ, float upX, float upY, float upZ,
            float yaw, float pitch);

  //---

  bool isRotate() const { return rotate_; }
  void setRotate(bool b) { rotate_ = b; }

  //---

  const CGLVector3D &position() const { return position_; }
  void setPosition(const CGLVector3D &p);

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  const CGLMatrix3D &getViewMatrix() const { return viewMatrix_; }

  //---

  float zoom() const { return zoom_; }
  void setZoom(float z);

  double near() const { return near_; }
  void setNear(double r) { near_ = r; }

  double far() const { return far_; }
  void setFar(double r) { far_ = r; }

  CGLMatrix3D getPerspectiveMatrix(float aspect=1.0) const;

  //---

  // processes input received from any keyboard-like input system.
  // Accepts input parameter in the form of camera defined ENUM
  // (to abstract it from windowing systems)
  void processKeyboard(Movement direction, float deltaTime);

  void moveLeft (float deltaTime=0.1);
  void moveRight(float deltaTime=0.1);
  void moveUp   (float deltaTime=0.1);
  void moveDown (float deltaTime=0.1);
  void moveIn   (float deltaTime=0.1);
  void moveOut  (float deltaTime=0.1);

  void rotateLeft (float deltaAngle=0.1);
  void rotateRight(float deltaAngle=0.1);
  void rotateUp   (float deltaAngle=0.1);
  void rotateDown (float deltaAngle=0.1);

  // processes input received from a mouse input system. Expects the offset value in
  // both the x and y direction.
  void processMouseMovement(float xoffset, float yoffset, bool constrainPitch=true);

  // processes input received from a mouse scroll-wheel event. Only requires input on
  // the vertical wheel-axis
  void processMouseScroll(float yoffset);

  void setLastPos(float x, float y);

  void deltaPos(float x, float y, float &dx, float &dy);

  bool isFirstPos() const { return firstPos_; }
  void setFirstPos(bool b);

  void updateFrameTime();

  float deltaTime() const;

  float yaw() const { return yaw_; }
  void setYaw(float a);

  float pitch() const { return pitch_; }
  void setPitch(float a);

  float roll() const { return roll_; }
  void setRoll(float r);

 private:
  void init(const CGLVector3D &position, const CGLVector3D &up, float yaw, float pitch);
  void init();

 private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors(bool rotate=false);

  void updateViewMatrix(bool rotate=false);

 private:
  // Default camera values
  static constexpr float YAW         = -90.0f;
  static constexpr float PITCH       =   0.0f;
  static constexpr float SPEED       =   2.5f;
  static constexpr float ROLL        =  90.0f;
  static constexpr float SENSITIVITY =   0.1f;
  static constexpr float ZOOM        =  45.0f;

  // camera Attributes
  CGLVector3D origin_ { 0.0f, 0.0f,  0.0f };
  CGLVector3D position_;
  CGLVector3D front_  { 0.0f, 0.0f, -1.0f };
  CGLVector3D up_;
  CGLVector3D right_;
  CGLVector3D worldUp_;

  // euler Angles
  float yaw_   { YAW };
  float pitch_ { PITCH };
  float roll_  { ROLL };

  // camera options
  float movementSpeed_    { SPEED };
  float mouseSensitivity_ { SENSITIVITY };

  float zoom_ { ZOOM };
  float near_ { 0.1 };
  float far_  { 100.0 };

  bool rotate_ { true };

  // movement data
  float lastX_ { 0.0 };
  float lastY_ { 0.0 };

  bool firstPos_ { true };

  // timing
  QDateTime startTime_;

  float deltaTime_ { 0.0f }; // time between current frame and last frame
  float lastFrame_ { 0.0f };

  CGLMatrix3D viewMatrix_;
};

#endif
