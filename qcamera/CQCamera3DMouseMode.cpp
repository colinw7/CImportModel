#include <CQCamera3DMouseMode.h>
#include <CQCamera3DOptions.h>
#include <CQCamera3DObjectData.h>

#include <CGeomCircle3D.h>
#include <CGeomCube3D.h>
#include <CGeomCylinder3D.h>
#include <CGeomPlane3D.h>
#include <CGeomPyramid3D.h>
#include <CGeomSphere3D.h>
#include <CGeomTorus3D.h>

#include <QKeyEvent>

#include <cassert>

CQCamera3DMouseModeMgr::
CQCamera3DMouseModeMgr(CQCamera3DWidget *widget) :
 widget_(widget)
{
}

CQCamera3DMouseMode *
CQCamera3DMouseModeMgr::
currentMode()
{
  if (! mouseModeStack_.empty())
    return mouseModeStack_.back();
  else
    return nullptr;
}

void
CQCamera3DMouseModeMgr::
startMode(CQCamera3DMouseMode *mode)
{
  mouseModeStack_.push_back(mode);

  mode->setMgr(this);

  mode->begin();
}

CQCamera3DMouseMode *
CQCamera3DMouseModeMgr::
endMode()
{
  assert(! mouseModeStack_.empty());

  auto *mode = mouseModeStack_.back();

  mouseModeStack_.pop_back();

  mode->end();

  mode->setMgr(nullptr);

  // delete mode ?

  return mode;
}

//---

CQCamera3DAddMouseMode::
CQCamera3DAddMouseMode(const AddType &type, CGeomObject3D *object) :
 type_(type), object_(object)
{
}

void
CQCamera3DAddMouseMode::
begin()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  position_ = canvas->cursor();
  rotation_ = CVector3D(0, 0, 0);
  size_     = CVector3D(1, 1, 1);

  power1_      = 1.0;
  power2_      = 1.0;
  torusRadius_ = 0.1;

  canvas->setOptions(new CQCamera3DAddOptions(this));

  canvas->showOptions();
}

void
CQCamera3DAddMouseMode::
end()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->hideOptions();

  canvas->setOptions(nullptr);
}

void
CQCamera3DAddMouseMode::
updateObject()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  object_->clearGeometry();

  CPoint3D origin(0, 0, 0);

  if      (type_ == AddType::CIRCLE)
    CGeomCircle3D::addGeometry(object_, origin, 1.0, circleConfig_);
  else if (type_ == AddType::CUBE)
    CGeomCube3D::addGeometry(object_, origin, 1.0);
  else if (type_ == AddType::CYLINDER)
    CGeomCylinder3D::addGeometry(object_, origin, 1.0, 1.0);
  else if (type_ == AddType::PLANE)
    CGeomPlane3D::addGeometry(object_, origin, 1.0, 1.0);
  else if (type_ == AddType::PYRAMID)
    CGeomPyramid3D::addGeometry(object_, origin, 1.0, 1.0);
  else if (type_ == AddType::SPHERE)
    CGeomSphere3D::addGeometry(object_, origin, 0.5, sphereConfig_);
  else if (type_ == AddType::TORUS)
    CGeomTorus3D::addGeometry(object_, origin, 1.0, torusRadius_, power1_, power2_, 30, 30);

  auto mt  = CMatrix3D::translation(position_.x, position_.y, position_.z);
  auto mrx = CMatrix3D::rotation(rotation_.getX(), CVector3D(1, 0, 0));
  auto mry = CMatrix3D::rotation(rotation_.getY(), CVector3D(0, 1, 0));
  auto mrz = CMatrix3D::rotation(rotation_.getZ(), CVector3D(0, 0, 1));
  auto ms  = CMatrix3D::scale(size_.getX(), size_.getY(), size_.getZ());

  auto m = mt*mrx*mry*mrz*ms;

  object_->transform(m);

  canvas->addObjectsData();
  canvas->update();
}

//---

CQCamera3DMoveMouseMode::
CQCamera3DMoveMouseMode(CGeomObject3D *object) :
 object_(object)
{
}

void
CQCamera3DMoveMouseMode::
begin()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->setOptions(new CQCamera3DMoveOptions(this));

  canvas->showOptions();
}

void
CQCamera3DMoveMouseMode::
end()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->hideOptions();

  canvas->setOptions(nullptr);

  canvas->setMoveDirection(CQCamera3DCanvas::MoveDirection::NONE);

  canvas->setEditType(CQCamera3DCanvas::EditType::SELECT);
}

void
CQCamera3DMoveMouseMode::
keyPress(QKeyEvent *e)
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  auto dir = canvas->moveDirection();

  auto *objectData = canvas->getObjectData(object_);

  auto bbox = objectData->bbox();

  auto d = bbox.getMaxSize()/100.0;

  auto k = e->key();

  if      (k == Qt::Key_Left) {
    if (dir == CQCamera3DCanvas::MoveDirection::X)
      canvas->moveObject(object_, CVector3D(-d, 0, 0));
  }
  else if (k == Qt::Key_Right) {
    if (dir == CQCamera3DCanvas::MoveDirection::X)
      canvas->moveObject(object_, CVector3D(d, 0, 0));
  }
  else if (k == Qt::Key_Up) {
    if      (dir == CQCamera3DCanvas::MoveDirection::Y)
      canvas->moveObject(object_, CVector3D(0, d, 0));
    else if (dir == CQCamera3DCanvas::MoveDirection::Z)
      canvas->moveObject(object_, CVector3D(0, 0, d));
  }
  else if (k == Qt::Key_Down) {
    if      (dir == CQCamera3DCanvas::MoveDirection::Y)
      canvas->moveObject(object_, CVector3D(0, -d, 0));
    else if (dir == CQCamera3DCanvas::MoveDirection::Z)
      canvas->moveObject(object_, CVector3D(0, 0, -d));
  }

  canvas->update();
}

//---

CQCamera3DScaleMouseMode::
CQCamera3DScaleMouseMode(CGeomObject3D *object) :
 object_(object)
{
}

void
CQCamera3DScaleMouseMode::
begin()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->setOptions(new CQCamera3DScaleOptions(this));

  canvas->showOptions();
}

void
CQCamera3DScaleMouseMode::
end()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->hideOptions();

  canvas->setOptions(nullptr);

  canvas->setMoveDirection(CQCamera3DCanvas::MoveDirection::NONE);

  canvas->setEditType(CQCamera3DCanvas::EditType::SELECT);
}

void
CQCamera3DScaleMouseMode::
keyPress(QKeyEvent *e)
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  auto dir = canvas->moveDirection();

  auto k = e->key();

  auto scale  = 1.1;
  auto iscale = 1.0/scale;

  if      (k == Qt::Key_Left) {
    if (dir == CQCamera3DCanvas::MoveDirection::X)
      canvas->scaleObject(object_, CVector3D(iscale, 1, 1));
  }
  else if (k == Qt::Key_Right) {
    if (dir == CQCamera3DCanvas::MoveDirection::X)
      canvas->scaleObject(object_, CVector3D(scale, 1, 1));
  }
  else if (k == Qt::Key_Up) {
    if      (dir == CQCamera3DCanvas::MoveDirection::Y)
      canvas->scaleObject(object_, CVector3D(1, scale, 1));
    else if (dir == CQCamera3DCanvas::MoveDirection::Z)
      canvas->scaleObject(object_, CVector3D(1, 1, scale));
  }
  else if (k == Qt::Key_Down) {
    if      (dir == CQCamera3DCanvas::MoveDirection::Y)
      canvas->scaleObject(object_, CVector3D(1, iscale, 1));
    else if (dir == CQCamera3DCanvas::MoveDirection::Z)
      canvas->scaleObject(object_, CVector3D(1, 1, iscale));
  }

  canvas->update();
}

//---

CQCamera3DRotateMouseMode::
CQCamera3DRotateMouseMode(CGeomObject3D *object) :
 object_(object)
{
}

void
CQCamera3DRotateMouseMode::
begin()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->setOptions(new CQCamera3DRotateOptions(this));

  canvas->showOptions();
}

void
CQCamera3DRotateMouseMode::
end()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->hideOptions();

  canvas->setOptions(nullptr);

  canvas->setMoveDirection(CQCamera3DCanvas::MoveDirection::NONE);

  canvas->setEditType(CQCamera3DCanvas::EditType::SELECT);
}

void
CQCamera3DRotateMouseMode::
keyPress(QKeyEvent *e)
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

#if 0
  auto *objectData = canvas->getObjectData(object_);

  auto bbox   = objectData->bbox();
  auto center = bbox.getCenter();
#endif

  auto dir = canvas->moveDirection();

  auto k = e->key();

  auto da = M_PI/180.0;

  CVector3D axis;

#if 0
  if      (dir == CQCamera3DCanvas::MoveDirection::X)
    axis = CVector3D(CPoint3D(bbox.getXMin(), center.y, center.z),
                     CPoint3D(bbox.getXMax(), center.y, center.z));
  else if (dir == CQCamera3DCanvas::MoveDirection::Y)
    axis = CVector3D(CPoint3D(center.x, bbox.getYMin(), center.x),
                     CPoint3D(center.x, bbox.getYMax(), center.x));
  else if (dir == CQCamera3DCanvas::MoveDirection::Z)
    axis = CVector3D(CPoint3D(center.x, center.y, bbox.getZMin()),
                     CPoint3D(center.x, center.y, bbox.getZMax()));
#else
  if      (dir == CQCamera3DCanvas::MoveDirection::X)
    axis = CVector3D(1, 0, 0);
  else if (dir == CQCamera3DCanvas::MoveDirection::Y)
    axis = CVector3D(0, 1, 0);
  else if (dir == CQCamera3DCanvas::MoveDirection::Z)
    axis = CVector3D(0, 0, 1);
#endif

  if      (k == Qt::Key_Left) {
    if      (dir == CQCamera3DCanvas::MoveDirection::Y)
      canvas->rotateObject(object_, -da, axis);
    else if (dir == CQCamera3DCanvas::MoveDirection::Z)
      canvas->rotateObject(object_, -da, axis);
  }
  else if (k == Qt::Key_Right) {
    if      (dir == CQCamera3DCanvas::MoveDirection::Y)
      canvas->rotateObject(object_, da, axis);
    else if (dir == CQCamera3DCanvas::MoveDirection::Z)
      canvas->rotateObject(object_, da, axis);
  }
  else if (k == Qt::Key_Up) {
    if (dir == CQCamera3DCanvas::MoveDirection::X)
      canvas->rotateObject(object_, da, axis);
  }
  else if (k == Qt::Key_Down) {
    if (dir == CQCamera3DCanvas::MoveDirection::X)
      canvas->rotateObject(object_, -da, axis);
  }

  canvas->update();
}

//---

CQCamera3DExtrudeMouseMode::
CQCamera3DExtrudeMouseMode()
{
}

void
CQCamera3DExtrudeMouseMode::
begin()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->setOptions(new CQCamera3DExtrudeOptions(this));

  canvas->showOptions();
}

void
CQCamera3DExtrudeMouseMode::
end()
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  canvas->hideOptions();

  canvas->setOptions(nullptr);

  canvas->setMoveDirection(CQCamera3DCanvas::MoveDirection::NONE);

  canvas->setEditType(CQCamera3DCanvas::EditType::SELECT);
}

void
CQCamera3DExtrudeMouseMode::
keyPress(QKeyEvent *e)
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mgr()->widget());

  auto bbox = canvas->bbox();

  if (e->key() == Qt::Key_Up) {
    canvas->extrudeMove(bbox.getMaxSize()/100.0);

    canvas->update();
  }
}
