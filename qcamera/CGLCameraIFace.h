#ifndef CGLCameraIFace_H
#define CGLCameraIFace_H

#include <CMatrix3DH.h>
#include <CVector3D.h>

#include <string>
#include <cassert>

class CGLCameraIFace {
 public:
  CGLCameraIFace() { }

  virtual ~CGLCameraIFace() { }

  //---

  const uint &id() const { return id_; }
  void setId(const uint &v) { id_ = v; }

  //---

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  bool isPerspective() const { return perspective_; }
  void setPerspective(bool b) { perspective_ = b; }

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  //---

  const std::string &name() const { return name_; }
  void setName(const std::string &s) { name_ = s; }

  //---

  // set perspective values

  // field of view
  double fov() const { return fov_; }
  virtual void setFov(double r) { fov_ = r; }

  // pixel aspect
  double aspect() const { return aspect_; }
  virtual void setAspect(double r) { aspect_ = r; }

  // near z
  double near() const { return near_; }
  virtual void setNear(double r) { near_ = r; }

  // far z
  double far() const { return far_; }
  virtual void setFar(double r) { far_ = r; }

  //---

  // origin (for rotation center)
  virtual const CVector3D &origin() const { return origin_; }
  virtual void setOrigin(const CVector3D &p) { origin_ = p; }

  // position (for camera position)
  virtual const CVector3D &position() const { return position_; }
  virtual void setPosition(const CVector3D &p) { position_ = p; }

  //---

  CPoint3D positionPoint() const { return position().point(); }

  CVector3D vector() const {
    return CVector3D(position(), origin());
  }

  double length() const {
    return vector().length();
  }

  //---

  // rotation x angle
  virtual double pitch() const { return pitch_; }
  virtual void setPitch(double r) { pitch_ = r; }

  // rotation x angle
  virtual double yaw() const { return yaw_; }
  virtual void setYaw(double r) { yaw_ = r; }

  // rotation z angle
  virtual double roll() const { return roll_; }
  virtual void setRoll(double r) { roll_ = r; }

  //---

  virtual CVector3D front() const = 0;
  virtual CVector3D up   () const = 0;
  virtual CVector3D right() const = 0;

  //---

  // get world matrix
  CMatrix3DH worldMatrix() const {
    if (isPerspective())
      return perspectiveMatrix();
    else
      return orthoMatrix();
  }

  // get perspective matrix
  virtual CMatrix3DH perspectiveMatrix() const = 0;

  // get ortho matrix
  virtual CMatrix3DH orthoMatrix() const = 0;

  // get view matrix
  virtual CMatrix3DH viewMatrix() const = 0;

  //---

  void moveFront(double d) {
    setPosition(position() + d*front());
  }

  void moveRight(double d) {
    setPosition(position() + d*right());
  }

  void moveUp(double d) {
    setPosition(position() + d*up());
  }

  //---

  // rotate around origin in x axis (pitch) - ZY
  virtual void moveAroundX(double d) = 0;

  // rotate around origin in y axis (yaw) - XZ
  virtual void moveAroundY(double d) = 0;

  // rotate around origin in z axis (roll) - XY
  virtual void moveAroundZ(double d) = 0;

  //---

  virtual void rotatePosition(int axis, double f) {
    if      (axis == 0) setPitch(pitch() + f);
    else if (axis == 1) setYaw  (yaw  () + f);
    else if (axis == 2) setRoll (roll () + f);
    else assert(false);
  }

  virtual void moveOrigin(int axis, double f) {
    if      (axis == 0) setOrigin(origin() + CVector3D(f, 0, 0));
    else if (axis == 1) setOrigin(origin() + CVector3D(0, f, 0));
    else if (axis == 2) setOrigin(origin() + CVector3D(0, 0, f));
    else assert(false);
  }

  virtual void movePosition(int axis, double f) {
    if      (axis == 0) setPosition(position() + CVector3D(f, 0, 0));
    else if (axis == 1) setPosition(position() + CVector3D(0, f, 0));
    else if (axis == 2) setPosition(position() + CVector3D(0, 0, f));
    else assert(false);
  }

  //---

  virtual void moveBy(const CVector3D &d) {
    origin_   += d;
    position_ += d;
  }

  //---

  virtual void print() {
    auto radToDeg = [](double a) { return 180.0*a/M_PI; };

    std::cerr << "Origin:   " << origin  () << "\n";
    std::cerr << "Position: " << position() << "\n";

    std::cerr << "Pitch:    " << radToDeg(pitch()) << "\n";
    std::cerr << "Yaw:      " << radToDeg(yaw  ()) << "\n";
    std::cerr << "Roll:     " << radToDeg(roll ()) << "\n";

    std::cerr << "Front:    " << front() << "\n";
    std::cerr << "Right:    " << right() << "\n";
    std::cerr << "Up:       " << up   () << "\n";
  }

  //---

  virtual void test() { }

 protected:
  uint id_ { 0 };

  bool visible_     { true };
  bool perspective_ { true };
  bool debug_       { false };

  std::string name_;

  double fov_    { 45.0 };     // field of view
  double aspect_ { 1.0 };      // pixel aspect
  double near_   { 0.1 };      // near z (too small get z fighting)
  double far_    { 100000.0 }; // far z (too large (too far fron near) get z fighting)

  CVector3D origin_   { 0.0, 0.0, 0.0 }; // camera rotation center
  CVector3D position_ { 1.0, 1.0, 1.0 }; // camera position

  double pitch_ { 0.0 };   // x axis rotation
  double yaw_   { -M_PI }; // y axis rotation
  double roll_  { 0.0 };   // z axis rotation
};

#endif
