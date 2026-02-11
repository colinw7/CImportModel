#include <CQCamera3DMouseMode.h>
#include <CQCamera3DOptions.h>
#include <CQCamera3DGeomObject.h>

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
CQCamera3DMouseModeMgr(CQCamera3DMouseModeIFace *iface) :
 iface_(iface)
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

void
CQCamera3DMouseModeMgr::
endAllModes()
{
  while (depth() > 0)
    (void) endMode();
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
  auto *iface = mgr()->iface();

  position_ = iface->cursor();
  rotation_ = CVector3D(0, 0, 0);
  size_     = CVector3D(1, 1, 1);

  power1_      = 1.0;
  power2_      = 1.0;
  torusRadius_ = 0.1;

  iface->setOptions(new CQCamera3DAddOptions(this));

  iface->showOptions();
}

void
CQCamera3DAddMouseMode::
end()
{
  auto *iface = mgr()->iface();

  iface->hideOptions();

  iface->setOptions(nullptr);
}

void
CQCamera3DAddMouseMode::
updateObject()
{
  auto *iface = mgr()->iface();

  iface->beginUpdateObjects();

  object_->clearGeometry();

  CPoint3D origin(0, 0, 0);

  if      (type_ == AddType::CIRCLE)
    CGeomCircle3D::addGeometry(object_, origin, 1.0, circleConfig_);
  else if (type_ == AddType::CUBE)
    CGeomCube3D::addGeometry(object_, origin, 1.0);
  else if (type_ == AddType::CYLINDER)
    CGeomCylinder3D::addGeometry(object_, origin, 1.0, 1.0, cylinderConfig_);
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

  iface->endUpdateObjects();
}

//---

CQCamera3DCursorMouseMode::
CQCamera3DCursorMouseMode(CGeomObject3D *object) :
 object_(object)
{
}

void
CQCamera3DCursorMouseMode::
begin()
{
  auto *iface = mgr()->iface();

  iface->setOptions(new CQCamera3DCursorOptions(this));

  iface->showOptions();
}

void
CQCamera3DCursorMouseMode::
end()
{
  auto *iface = mgr()->iface();

  iface->hideOptions();

  iface->resetOptions();
}

void
CQCamera3DCursorMouseMode::
mousePress(QMouseEvent *e)
{
  auto *iface = mgr()->iface();

  auto p = CPoint2D(e->x(), e->y());

  iface->moveCursorToMouse(CPoint3D(p));
}

void
CQCamera3DCursorMouseMode::
keyPress(QKeyEvent *e)
{
  auto *iface = mgr()->iface();

  auto dir = iface->moveDirection();

  auto bbox = iface->bbox();

  auto d = bbox.getMaxSize()/100.0;

  auto moveCursor = [&](const CVector3D &d) {
    auto pos = iface->cursor();

    iface->setCursor(pos + d);
  };

  if      (e->key() == Qt::Key_Left) {
    if (dir == MoveDirection::X)
      moveCursor(CVector3D(-d, 0, 0));
  }
  else if (e->key() == Qt::Key_Right) {
    if (dir == MoveDirection::X)
      moveCursor(CVector3D(d, 0, 0));
  }
  else if (e->key() == Qt::Key_Up) {
    if      (dir == MoveDirection::Y)
      moveCursor(CVector3D(0, d, 0));
    else if (dir == MoveDirection::Z)
      moveCursor(CVector3D(0, 0, d));
  }
  else if (e->key() == Qt::Key_Down) {
    if      (dir == MoveDirection::Y)
      moveCursor(CVector3D(0, -d, 0));
    else if (dir == MoveDirection::Z)
      moveCursor(CVector3D(0, 0, -d));
  }

  options_->updateWidgets();
}

//---

CQCamera3DMoveMouseMode::
CQCamera3DMoveMouseMode()
{
}

void
CQCamera3DMoveMouseMode::
begin()
{
  auto *iface = mgr()->iface();

  selectData_ = iface->getSelection();

  iface->setOptions(new CQCamera3DMoveOptions(this));

  iface->showOptions();
}

void
CQCamera3DMoveMouseMode::
end()
{
  auto *iface = mgr()->iface();

  iface->hideOptions();

  iface->resetOptions();
}

void
CQCamera3DMoveMouseMode::
keyPress(QKeyEvent *e)
{
  auto *iface = mgr()->iface();

  if      (e->key() == Qt::Key_X) {
    iface->setMoveDirection(MoveDirection::X);
    options_->updateWidgets();
    return;
  }
  else if (e->key() == Qt::Key_Y) {
    iface->setMoveDirection(MoveDirection::Y);
    options_->updateWidgets();
    return;
  }
  else if (e->key() == Qt::Key_Z) {
    iface->setMoveDirection(MoveDirection::Z);
    options_->updateWidgets();
    return;
  }

  auto dir = iface->moveDirection();

  auto bbox = iface->bbox();

  auto d = bbox.getMaxSize()/100.0;

  if      (selectData_.type == CQCamera3DSelectType::OBJECT) {
    for (auto *object : selectData_.objects) {
      if (! iface->is2D()) {
        if      (e->key() == Qt::Key_Left) {
          if (dir == MoveDirection::X)
            iface->moveObject(object, CVector3D(-d, 0, 0));
        }
        else if (e->key() == Qt::Key_Right) {
          if (dir == MoveDirection::X)
            iface->moveObject(object, CVector3D(d, 0, 0));
        }
        else if (e->key() == Qt::Key_Up) {
          if      (dir == MoveDirection::Y)
            iface->moveObject(object, CVector3D(0, d, 0));
          else if (dir == MoveDirection::Z)
            iface->moveObject(object, CVector3D(0, 0, d));
        }
        else if (e->key() == Qt::Key_Down) {
          if      (dir == MoveDirection::Y)
            iface->moveObject(object, CVector3D(0, -d, 0));
          else if (dir == MoveDirection::Z)
            iface->moveObject(object, CVector3D(0, 0, -d));
        }
      }
      else {
        if      (e->key() == Qt::Key_Left) {
          iface->moveObject(object, CVector3D(-d, 0, 0));
        }
        else if (e->key() == Qt::Key_Right) {
          iface->moveObject(object, CVector3D(d, 0, 0));
        }
        else if (e->key() == Qt::Key_Up) {
          iface->moveObject(object, CVector3D(0, d, 0));
        }
        else if (e->key() == Qt::Key_Down) {
          iface->moveObject(object, CVector3D(0, -d, 0));
        }
      }
    }
  }
  else if (selectData_.type == CQCamera3DSelectType::FACE) {
    iface->beginUpdateObjects();

    for (auto *face : selectData_.faces) {
      if (! iface->is2D()) {
        if      (e->key() == Qt::Key_Left) {
          if (dir == MoveDirection::X)
            iface->moveFace(face, CVector3D(-d, 0, 0));
        }
        else if (e->key() == Qt::Key_Right) {
          if (dir == MoveDirection::X)
            iface->moveFace(face, CVector3D(d, 0, 0));
        }
        else if (e->key() == Qt::Key_Up) {
          if      (dir == MoveDirection::Y)
            iface->moveFace(face, CVector3D(0, d, 0));
          else if (dir == MoveDirection::Z)
            iface->moveFace(face, CVector3D(0, 0, d));
        }
        else if (e->key() == Qt::Key_Down) {
          if      (dir == MoveDirection::Y)
            iface->moveFace(face, CVector3D(0, -d, 0));
          else if (dir == MoveDirection::Z)
            iface->moveFace(face, CVector3D(0, 0, -d));
        }
      }
      else {
        if      (e->key() == Qt::Key_Left) {
          iface->moveFace(face, CVector3D(-d, 0, 0));
        }
        else if (e->key() == Qt::Key_Right) {
          iface->moveFace(face, CVector3D(d, 0, 0));
        }
        else if (e->key() == Qt::Key_Up) {
          iface->moveFace(face, CVector3D(0, d, 0));
        }
        else if (e->key() == Qt::Key_Down) {
          iface->moveFace(face, CVector3D(0, -d, 0));
        }
      }
    }

    iface->endUpdateObjects();
  }

  options_->updateWidgets();
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
  auto *iface = mgr()->iface();

  iface->setOptions(new CQCamera3DScaleOptions(this));

  iface->showOptions();
}

void
CQCamera3DScaleMouseMode::
end()
{
  auto *iface = mgr()->iface();

  iface->hideOptions();

  iface->resetOptions();
}

void
CQCamera3DScaleMouseMode::
keyPress(QKeyEvent *e)
{
  auto *iface = mgr()->iface();

  auto dir = iface->moveDirection();

  auto scale  = 1.1;
  auto iscale = 1.0/scale;

  if      (e->key() == Qt::Key_Left) {
    if (dir == MoveDirection::X)
      iface->scaleObject(object_, CVector3D(iscale, 1, 1));
  }
  else if (e->key() == Qt::Key_Right) {
    if (dir == MoveDirection::X)
      iface->scaleObject(object_, CVector3D(scale, 1, 1));
  }
  else if (e->key() == Qt::Key_Up) {
    if      (dir == MoveDirection::Y)
      iface->scaleObject(object_, CVector3D(1, scale, 1));
    else if (dir == MoveDirection::Z)
      iface->scaleObject(object_, CVector3D(1, 1, scale));
  }
  else if (e->key() == Qt::Key_Down) {
    if      (dir == MoveDirection::Y)
      iface->scaleObject(object_, CVector3D(1, iscale, 1));
    else if (dir == MoveDirection::Z)
      iface->scaleObject(object_, CVector3D(1, 1, iscale));
  }

  options_->updateWidgets();
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
  auto *iface = mgr()->iface();

  iface->setOptions(new CQCamera3DRotateOptions(this));

  iface->showOptions();
}

void
CQCamera3DRotateMouseMode::
end()
{
  auto *iface = mgr()->iface();

  iface->hideOptions();

  iface->resetOptions();
}

void
CQCamera3DRotateMouseMode::
keyPress(QKeyEvent *e)
{
  auto *iface = mgr()->iface();

#if 0
  auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object_);

  auto bbox   = iface->bbox();
  auto center = bbox.getCenter();
#endif

  auto dir = iface->moveDirection();

  auto da = M_PI/180.0;

  CVector3D axis;

#if 0
  if      (dir == MoveDirection::X)
    axis = CVector3D(CPoint3D(bbox.getXMin(), center.y, center.z),
                     CPoint3D(bbox.getXMax(), center.y, center.z));
  else if (dir == MoveDirection::Y)
    axis = CVector3D(CPoint3D(center.x, bbox.getYMin(), center.x),
                     CPoint3D(center.x, bbox.getYMax(), center.x));
  else if (dir == MoveDirection::Z)
    axis = CVector3D(CPoint3D(center.x, center.y, bbox.getZMin()),
                     CPoint3D(center.x, center.y, bbox.getZMax()));
#else
  if      (dir == MoveDirection::X)
    axis = CVector3D(1, 0, 0);
  else if (dir == MoveDirection::Y)
    axis = CVector3D(0, 1, 0);
  else if (dir == MoveDirection::Z)
    axis = CVector3D(0, 0, 1);
#endif

  if      (e->key() == Qt::Key_Left) {
    if      (dir == MoveDirection::Y)
      iface->rotateObject(object_, -da, axis);
    else if (dir == MoveDirection::Z)
      iface->rotateObject(object_, -da, axis);
  }
  else if (e->key() == Qt::Key_Right) {
    if      (dir == MoveDirection::Y)
      iface->rotateObject(object_, da, axis);
    else if (dir == MoveDirection::Z)
      iface->rotateObject(object_, da, axis);
  }
  else if (e->key() == Qt::Key_Up) {
    if (dir == MoveDirection::X)
      iface->rotateObject(object_, da, axis);
  }
  else if (e->key() == Qt::Key_Down) {
    if (dir == MoveDirection::X)
      iface->rotateObject(object_, -da, axis);
  }

  options_->updateWidgets();
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
  auto *iface = mgr()->iface();

  iface->setOptions(new CQCamera3DExtrudeOptions(this));

  iface->showOptions();
}

void
CQCamera3DExtrudeMouseMode::
end()
{
  auto *iface = mgr()->iface();

  iface->hideOptions();

  iface->resetOptions();
}

void
CQCamera3DExtrudeMouseMode::
keyPress(QKeyEvent *e)
{
  auto *iface = mgr()->iface();

  auto bbox = iface->bbox();

  if (e->key() == Qt::Key_Up)
    iface->extrudeMove(bbox.getMaxSize()/100.0);

  options_->updateWidgets();
}
