#ifndef CGL_CAMERA_H
#define CGL_CAMERA_H

#include <CGLMatrix3D.h>
#include <CGLVector3D.h>
#include <CQuaternion.h>

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
    DOWN,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    STRAFE_LEFT,
    STRAFE_RIGHT
  };

  enum class RotateAt {
    ORIGIN,
    POSITION
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

  CGLCamera(const CGLVector3D &origin   = CGLVector3D(0.0f, 0.0f, 0.0f),
            const CGLVector3D &position = CGLVector3D(1.0f, 1.0f, 1.0f),
            const CGLVector3D &up       = CGLVector3D(0.0f, 1.0f, 0.0f));

  virtual ~CGLCamera() { }

  //---

  bool isRotate() const { return rotate_; }
  void setRotate(bool b) { rotate_ = b; }

  const RotateAt &rotateAt() const { return rotateAt_; }
  void setRotateAt(const RotateAt &r) { rotateAt_ = r; }

  bool isStrafe() const { return strafe_; }
  void setStrafe(bool b) { strafe_ = b; }

  //---

  const CGLVector3D &origin() const { return origin_; }
  void setOrigin(const CGLVector3D &p);

  const CGLVector3D &position() const { return position_; }
  void setPosition(const CGLVector3D &p);

  const CGLVector3D &up() const { return up_; }
  void setUp(const CGLVector3D &p);

  const CGLVector3D &right() const { return right_; }
  void setRight(const CGLVector3D &p);

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
  CGLMatrix3D getOrthoMatrix() const;

  //---

  struct ProcessKeyData {
    ProcessKeyData() { }

    float               deltaTime { 0.1 };
    bool                rotate    { false };
    bool                strafe    { false };
    CGLCamera::RotateAt rotateAt  { CGLCamera::RotateAt::ORIGIN };
  };

  // processes input received from any keyboard-like input system.
  // Accepts input parameter in the form of camera defined ENUM
  // (to abstract it from windowing systems)
  void processKeyboard(Movement direction, const ProcessKeyData &data=ProcessKeyData());

  void moveLeft (float deltaTime, bool isStrafe, const CGLCamera::RotateAt &rotateAt);
  void moveRight(float deltaTime, bool isStrafe, const CGLCamera::RotateAt &rotateAt);

  void moveUp  (float deltaTime, bool isStrafe, const CGLCamera::RotateAt &rotateAt);
  void moveDown(float deltaTime, bool isStrafe, const CGLCamera::RotateAt &rotateAt);

  void moveIn (float deltaTime, bool isStrafe, const CGLCamera::RotateAt &rotateAt);
  void moveOut(float deltaTime, bool isStrafe, const CGLCamera::RotateAt &rotateAt);

  void rotateLeft (float deltaAngle, const CGLCamera::RotateAt &rotateAt);
  void rotateRight(float deltaAngle, const CGLCamera::RotateAt &rotateAt);

  void rotateUp     (float deltaAngle=0.1);
  void rotateDown   (float deltaAngle=0.1);
  void rotateBack   (float deltaAngle=0.1);
  void rotateForward(float deltaAngle=0.1);

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

  float deltaTime() const { return deltaTime_; }

  float yaw() const { return yaw_; }
  void setYaw(float a);

  float pitch() const { return pitch_; }
  void setPitch(float a);

  float roll() const { return roll_; }
  void setRoll(float r);

  void updateRoll();

  //---

  float movementSpeed() const { return movementSpeed_; }
  void setMovementSpeed(float r) { movementSpeed_ = r; }

  float mouseSensitivity() const { return mouseSensitivity_; }
  void setMouseSensitivity(float r) { mouseSensitivity_ = r; }

  //---

  CQuaternion trackBall(CVector3D &cop, CVector3D &cor, CVector3D &dir1, CVector3D &dir2) const;

 protected:
  virtual void viewChanged() { }

 protected:
  void init(const CGLVector3D &position, const CGLVector3D &up, float yaw, float pitch);
  void init(const CGLVector3D &origin, const CGLVector3D &position, const CGLVector3D &up);
  void init();

 private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors(bool rotate);

  void updateCameraVectorsI(bool rotate);

  void updateViewMatrix(bool rotate);

 private:
  // Default camera values
  static constexpr float YAW         = -90.0f;
  static constexpr float PITCH       =   0.0f;
  static constexpr float SPEED       =   2.5f;
  static constexpr float ROLL        =  90.0f;
  static constexpr float SENSITIVITY =   0.1f;
  static constexpr float ZOOM        =  45.0f;

  // camera look at, position, and up vector
  CGLVector3D origin_   { 0.0f, 0.0f, 0.0f };
  CGLVector3D position_ { 1.0f, 1.0f, 1.0f };
  CGLVector3D up_       { 0.0f, 1.0f, 0.0f };

  CGLVector3D worldUp_; // from roll

  // calculated vectors (normalized)
  CGLVector3D front_ { 0.0f, 0.0f, -1.0f };
  CGLVector3D right_ { 0.0f, 1.0f, 0.0f };

  // euler Angles
  float yaw_   { YAW };   // camera look at x angle
  float pitch_ { PITCH }; // camera look at y angle
  float roll_  { ROLL };  // camera spin (perp to look vector)

  // camera options
  float movementSpeed_    { SPEED };
  float mouseSensitivity_ { SENSITIVITY };

  float zoom_ { ZOOM };
  float near_ { 0.001 };
  float far_  { 1000.0 };

  bool     rotate_   { true };
  RotateAt rotateAt_ { RotateAt::ORIGIN };
  bool     strafe_   { false };

  // movement data
  float lastX_ { 0.0 };
  float lastY_ { 0.0 };

  bool firstPos_ { true };

  // timing
  QDateTime startTime_;

  float deltaTime_ { 0.1f }; // time between current frame and last frame
  float lastFrame_ { 0.0f };

  CGLMatrix3D viewMatrix_;
};

#endif
