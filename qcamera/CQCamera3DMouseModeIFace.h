#ifndef CQCamera3DMouseModeIFace_H
#define CQCamera3DMouseModeIFace_H

#include <CQCamera3DApp.h>

class CQCamera3DOptions;

class CGeomObject3D;
class CGeomFace3D;

class CQCamera3DMouseModeIFace {
 public:
  using MoveDirection = CQCamera3DMoveDirection;
  using EditType      = CQCamera3DEditType;

 public:
  CQCamera3DMouseModeIFace() { }

  virtual ~CQCamera3DMouseModeIFace() { }

  //---

  // get/set cursor position
  virtual const CPoint3D &cursor() const = 0;
  virtual void setCursor(const CPoint3D &c) = 0;

  virtual void moveCursorToMouse(const CPoint3D &c) = 0;

  //---

  // get current selection
  virtual CQCamera3DSelectData getSelection() const = 0;

  //---

  // get bounding box
  virtual const CBBox3D &bbox() const = 0;

  //---

  // get/set move direction
  virtual const MoveDirection &moveDirection() const = 0;
  virtual void setMoveDirection(const MoveDirection &d) = 0;

  //---

  // modify objects
  virtual bool is2D() const { return false; }

  virtual void moveObject(CGeomObject3D *object, const CVector3D &d) = 0;
  virtual void moveFace(CGeomFace3D *face, const CVector3D &d) = 0;

  virtual void scaleObject(CGeomObject3D *object, const CVector3D &d) = 0;

  virtual void rotateObject(CGeomObject3D *object, double a, const CVector3D &d) = 0;

  //---

  // extrude
  double extrudeMoveDelta() const { return extrudeMoveDelta_; }
  void setExtrudeMoveDelta(double r) { extrudeMoveDelta_ = r; }

  virtual void extrudeMove(double d) = 0;

  //----

  // update objects
  virtual void beginUpdateObjects() { }
  virtual void endUpdateObjects() = 0;

  //---

  // options dialog
  virtual void setOptions(CQCamera3DOptions *options) = 0;

  virtual void resetOptions() = 0;

  virtual void showOptions() = 0;
  virtual void hideOptions() = 0;

 protected:
  double extrudeMoveDelta_ { 0.0 };
};

#endif
