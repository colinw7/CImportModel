#ifndef CGLCameraIFace_H
#define CGLCameraIFace_H

#include <CMatrix3DH.h>
#include <CVector3D.h>

#include <string>
#include <cassert>

class CGLCameraIFace {
 protected:
  template<typename T>
  void testAndSet(T &t, const T &v) {
    if (v != t) {
      t = v;

      stateChanged();
    }
  }

  double fixAngle(double a) const {
    const double pi2 = 2.0*M_PI;

    while (a < 0.0) a += pi2;
    while (a > pi2) a -= pi2;

    return a;
  }

  double degToRad(double a) const { return M_PI*a/180.0; };
  double radToDeg(double a) const { return 180.0*a/M_PI; }

 public:
  CGLCameraIFace() { }

  virtual ~CGLCameraIFace() { }

  //---

  const uint &id() const { return id_; }
  void setId(const uint &v) { testAndSet(id_, v); }

  //---

  bool isVisible() const { return visible_; }
  virtual void setVisible(bool b) { testAndSet(visible_, b); }

  bool isPerspective() const { return perspective_; }
  virtual void setPerspective(bool b) { testAndSet(perspective_, b); }

  bool isDebug() const { return debug_; }
  virtual void setDebug(bool b) { testAndSet(debug_, b); }

  //---

  const std::string &name() const { return name_; }
  void setName(const std::string &s) { testAndSet(name_, s); }

  //---

  // set perspective values

  // field of view
  double fov() const { return fov_; }
  virtual void setFov(double r) { testAndSet(fov_, r); }

  // pixel aspect
  double aspect() const { return aspect_; }
  virtual void setAspect(double r) { testAndSet(aspect_, r); }

  // near z
  double near() const { return near_; }
  virtual void setNear(double r) { testAndSet(near_, r); }

  // far z
  double far() const { return far_; }
  virtual void setFar(double r) { testAndSet(far_, r); }

  //---

  // origin (for rotation center)
  virtual const CVector3D &origin() const { return origin_; }
  virtual void setOrigin(const CVector3D &p) { testAndSet(origin_, p); }

  // position (for camera position)
  virtual const CVector3D &position() const { return position_; }
  virtual void setPosition(const CVector3D &p) { testAndSet(position_, p); }

  //---

  // postion/origin/dir helpers
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
  virtual void setPitch(double r) { testAndSet(pitch_, fixAngle(r)); }

  // rotation x angle
  virtual double yaw() const { return yaw_; }
  virtual void setYaw(double r) { testAndSet(yaw_, fixAngle(r)); }

  // rotation z angle
  virtual double roll() const { return roll_; }
  virtual void setRoll(double r) { testAndSet(roll_, fixAngle(r)); }

  //---

  // coordinate system vectors
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

  // move position

  virtual void moveFront(double d) {
    setPosition(position() + d*front());
  }

  virtual void moveRight(double d) {
    setPosition(position() + d*right());
  }

  virtual void moveUp(double d) {
    setPosition(position() + d*up());
  }

  //---

  // rotate around origin in x axis (pitch) - ZY
  virtual void moveAroundX(double /*d*/) { }

  // rotate around origin in y axis (yaw) - XZ
  virtual void moveAroundY(double /*d*/) { }

  // rotate around origin in z axis (roll) - XY
  virtual void moveAroundZ(double /*d*/) { }

  //---

  virtual void rotateX(double f) { setPitch(pitch() + f); }
  virtual void rotateY(double f) { setYaw  (yaw  () + f); }
  virtual void rotateZ(double f) { setRoll (roll () + f); }

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

    stateChanged();
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

  virtual void stateChanged() { }

  virtual void test() { }

 protected:
  uint id_ { 0 };

  bool visible_     { true };
  bool perspective_ { true };
  bool debug_       { false };

  std::string name_;

  double fov_    { 70.0 };    // field of view
  double aspect_ { 1.0 };     // pixel aspect
  double near_   { 0.01 };    // near z (too small get z fighting)
  double far_    { 10000.0 }; // far z (too large (too far fron near) get z fighting)

  CVector3D origin_   { 0.0, 0.0, 0.0 }; // camera rotation center
  CVector3D position_ { 1.0, 1.0, 1.0 }; // camera position

  double pitch_ { 0.0 };   // x axis rotation
  double yaw_   { -M_PI }; // y axis rotation
  double roll_  { 0.0 };   // z axis rotation
};

#endif
