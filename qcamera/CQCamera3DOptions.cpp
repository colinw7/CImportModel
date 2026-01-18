#include <CQCamera3DOptions.h>
#include <CQCamera3DUI.h>
#include <CQCamera3DMouseMode.h>
#include <CQCamera3DObjectData.h>
#include <CQCamera3DUtil.h>

#include <CGeomCircle3D.h>

#include <CQRealSpin.h>
#include <CQIntegerSpin.h>

#include <QComboBox>

namespace {

class CircleTypeInd : public ValueMap<CGeomCircle3D::Type, int> {
 public:
  CircleTypeInd() {
    add("Line", CGeomCircle3D::Type::LINE, 0);
    add("NGon", CGeomCircle3D::Type::NGON, 1);
    add("Fan" , CGeomCircle3D::Type::FAN , 2);
  };

  CGeomCircle3D::Type indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CGeomCircle3D::Type &type) {
    return lookup(type);
  }
};

CircleTypeInd circleTypeInd;

class MoveDirectionInd : public ValueMap<CQCamera3DCanvas::MoveDirection, int> {
 public:
  MoveDirectionInd() {
    add("None", CQCamera3DCanvas::MoveDirection::NONE, 0);
    add("X"   , CQCamera3DCanvas::MoveDirection::X   , 1);
    add("Y"   , CQCamera3DCanvas::MoveDirection::Y   , 2);
    add("Z"   , CQCamera3DCanvas::MoveDirection::Z   , 3);
  };

  CQCamera3DCanvas::MoveDirection indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CQCamera3DCanvas::MoveDirection &dir) {
    return lookup(dir);
  }
};

MoveDirectionInd moveDirectionInd;

}

CQCamera3DAddOptions::
CQCamera3DAddOptions(CQCamera3DAddMouseMode *mode) :
 mode_(mode)
{
}

void
CQCamera3DAddOptions::
addOptions(CQCamera3DUI *ui)
{
  auto type = mode_->type();
  auto pos  = mode_->position();
  auto size = mode_->size();
  auto tr   = mode_->torusRadius();

  ui->startGroup("Location");

  xEdit_ = ui->addLabelEdit("X", new CQRealSpin);
  yEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
  zEdit_ = ui->addLabelEdit("Z", new CQRealSpin);

  ui->endGroup();

  // TODO: align

  ui->startGroup("Rotation");

  rxEdit_ = ui->addLabelEdit("X", new CQRealSpin);
  ryEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
  rzEdit_ = ui->addLabelEdit("Z", new CQRealSpin);

  ui->endGroup();

  xEdit_->setValue(pos.x);
  yEdit_->setValue(pos.y);
  zEdit_->setValue(pos.z);

  ui->startGroup("Size");

  if      (type == CQCamera3DAddMouseMode::AddType::CIRCLE) {
    xSizeEdit_ = ui->addLabelEdit("X", new CQRealSpin);
    ySizeEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::CUBE) {
    xSizeEdit_ = ui->addLabelEdit("X", new CQRealSpin);
    ySizeEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
    zSizeEdit_ = ui->addLabelEdit("Z", new CQRealSpin);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::PLANE) {
    xSizeEdit_ = ui->addLabelEdit("Width" , new CQRealSpin);
    ySizeEdit_ = ui->addLabelEdit("Height", new CQRealSpin);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::SPHERE) {
    xSizeEdit_ = ui->addLabelEdit("X", new CQRealSpin);
    ySizeEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
    zSizeEdit_ = ui->addLabelEdit("Z", new CQRealSpin);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::TORUS) {
    innerRadiusEdit_ = ui->addLabelEdit("Inner Radius", new CQRealSpin);
    outerRadiusEdit_ = ui->addLabelEdit("Outer Radius", new CQRealSpin);

    ui->endGroup();

    innerRadiusEdit_->setValue(size.getX());
    outerRadiusEdit_->setValue(tr);
  }

  ui->endGroup();

  if (xSizeEdit_) xSizeEdit_->setValue(size.getX());
  if (ySizeEdit_) ySizeEdit_->setValue(size.getY());
  if (zSizeEdit_) zSizeEdit_->setValue(size.getZ());

  if      (type == CQCamera3DAddMouseMode::AddType::CIRCLE) {
    ui->startGroup("Config");

    circleTypeCombo_ = ui->addLabelEdit("Type"        , new QComboBox);
    verticesXEdit_   = ui->addLabelEdit("Vertices"    , new CQIntegerSpin);
    startAngleEdit_  = ui->addLabelEdit("Start Angle" , new CQRealSpin);
    extentAngleEdit_ = ui->addLabelEdit("Angle Extent", new CQRealSpin);

    circleTypeCombo_->addItems(circleTypeInd.names());

    ui->endGroup();

    const auto &data = mode_->circleConfig();

    circleTypeCombo_->setCurrentIndex(circleTypeInd.typeToInd(data.type));
    verticesXEdit_  ->setValue(data.num_xy);
    startAngleEdit_ ->setValue(data.angleStart);
    extentAngleEdit_->setValue(data.angleDelta);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::SPHERE) {
    ui->startGroup("Config");

    verticesXEdit_   = ui->addLabelEdit("Segments", new CQIntegerSpin);
    verticesYEdit_   = ui->addLabelEdit("Rings", new CQIntegerSpin);
    startAngleEdit_  = ui->addLabelEdit("Start Angle" , new CQRealSpin);
    extentAngleEdit_ = ui->addLabelEdit("Angle Extent", new CQRealSpin);

    ui->endGroup();

    const auto &data = mode_->sphereConfig();

    verticesXEdit_  ->setValue(data.num_xy);
    verticesYEdit_  ->setValue(data.num_patches);
    startAngleEdit_ ->setValue(data.angleStart);
    extentAngleEdit_->setValue(data.angleDelta);
  }

  connectSlots(true);
}

void
CQCamera3DAddOptions::
connectSlots(bool b)
{
  auto type = mode_->type();

  CQUtil::connectDisconnect(b, xEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xSlot(double)));
  CQUtil::connectDisconnect(b, yEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(ySlot(double)));
  CQUtil::connectDisconnect(b, zEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zSlot(double)));

  CQUtil::connectDisconnect(b, rxEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(rxSlot(double)));
  CQUtil::connectDisconnect(b, ryEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(rySlot(double)));
  CQUtil::connectDisconnect(b, rzEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(rzSlot(double)));

  if (xSizeEdit_)
    CQUtil::connectDisconnect(b, xSizeEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(xSizeSlot(double)));
  if (ySizeEdit_)
    CQUtil::connectDisconnect(b, ySizeEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(ySizeSlot(double)));
  if (zSizeEdit_)
    CQUtil::connectDisconnect(b, zSizeEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(zSizeSlot(double)));

  if (type == CQCamera3DAddMouseMode::AddType::TORUS) {
    CQUtil::connectDisconnect(b, innerRadiusEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(innerRadiusSlot(double)));
    CQUtil::connectDisconnect(b, outerRadiusEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(outerRadiusSlot(double)));
  }

  if      (type == CQCamera3DAddMouseMode::AddType::CIRCLE) {
    CQUtil::connectDisconnect(b, circleTypeCombo_, SIGNAL(currentIndexChanged(int)),
                              this, SLOT(circleTypeSlot(int)));
    CQUtil::connectDisconnect(b, verticesXEdit_, SIGNAL(valueChanged(int)),
                              this, SLOT(verticesXSlot(int)));
    CQUtil::connectDisconnect(b, startAngleEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(startAngleSlot(double)));
    CQUtil::connectDisconnect(b, extentAngleEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(extentAngleSlot(double)));
  }
  else if (type == CQCamera3DAddMouseMode::AddType::SPHERE) {
    CQUtil::connectDisconnect(b, verticesXEdit_, SIGNAL(valueChanged(int)),
                              this, SLOT(verticesXSlot(int)));
    CQUtil::connectDisconnect(b, verticesYEdit_, SIGNAL(valueChanged(int)),
                              this, SLOT(verticesYSlot(int)));
    CQUtil::connectDisconnect(b, startAngleEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(startAngleSlot(double)));
    CQUtil::connectDisconnect(b, extentAngleEdit_, SIGNAL(realValueChanged(double)),
                              this, SLOT(extentAngleSlot(double)));
  }
}

QString
CQCamera3DAddOptions::
title() const
{
  auto type = mode_->type();

  if      (type == CQCamera3DAddMouseMode::AddType::CIRCLE)
    return "Add Circle";
  else if (type == CQCamera3DAddMouseMode::AddType::CUBE)
    return "Add Cube";
  else if (type == CQCamera3DAddMouseMode::AddType::CYLINDER)
    return "Add Cylinder";
  else if (type == CQCamera3DAddMouseMode::AddType::PLANE)
    return "Add Plane";
  else if (type == CQCamera3DAddMouseMode::AddType::PYRAMID)
    return "Add Pyramid";
  else if (type == CQCamera3DAddMouseMode::AddType::SPHERE)
    return "Add Spehere";
  else if (type == CQCamera3DAddMouseMode::AddType::TORUS)
    return "Add Torus";
  else
    return "";
}

void
CQCamera3DAddOptions::
xSlot(double r)
{
  auto pos = mode_->position();

  pos.x = r;

  mode_->setPosition(pos);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
ySlot(double r)
{
  auto pos = mode_->position();

  pos.y = r;

  mode_->setPosition(pos);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
zSlot(double r)
{
  auto pos = mode_->position();

  pos.z = r;

  mode_->setPosition(pos);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
rxSlot(double r)
{
  auto v = mode_->rotation();

  v.setX(r);

  mode_->setRotation(v);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
rySlot(double r)
{
  auto v = mode_->rotation();

  v.setY(r);

  mode_->setRotation(v);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
rzSlot(double r)
{
  auto v = mode_->rotation();

  v.setZ(r);

  mode_->setRotation(v);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
xSizeSlot(double r)
{
  auto s = mode_->size();

  s.setX(r);

  mode_->setSize(s);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
ySizeSlot(double r)
{
  auto s = mode_->size();

  s.setY(r);

  mode_->setSize(s);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
zSizeSlot(double r)
{
  auto s = mode_->size();

  s.setZ(r);

  mode_->setSize(s);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
innerRadiusSlot(double r)
{
  xSizeSlot(r);
}

void
CQCamera3DAddOptions::
outerRadiusSlot(double r)
{
  mode_->setTorusRadius(r);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
circleTypeSlot(int i)
{
  auto data = mode_->circleConfig();

  data.type = circleTypeInd.indToType(i);

  mode_->setCircleConfig(data);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
verticesXSlot(int i)
{
  auto type = mode_->type();

  if      (type == CQCamera3DAddMouseMode::AddType::CIRCLE) {
    auto data = mode_->circleConfig();

    data.num_xy = std::max(i, 4);

    mode_->setCircleConfig(data);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::SPHERE) {
    auto data = mode_->sphereConfig();

    data.num_xy = std::max(i, 4);

    mode_->setSphereConfig(data);
  }

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
verticesYSlot(int i)
{
  auto data = mode_->sphereConfig();

  data.num_patches = std::max(i, 4);

  mode_->setSphereConfig(data);

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
startAngleSlot(double r)
{
  auto type = mode_->type();

  if      (type == CQCamera3DAddMouseMode::AddType::CIRCLE) {
    auto data = mode_->circleConfig();

    data.angleStart = r;

    mode_->setCircleConfig(data);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::SPHERE) {
    auto data = mode_->sphereConfig();

    data.angleStart = r;

    mode_->setSphereConfig(data);
  }

  mode_->updateObject();
}

void
CQCamera3DAddOptions::
extentAngleSlot(double r)
{
  auto type = mode_->type();

  if      (type == CQCamera3DAddMouseMode::AddType::CIRCLE) {
    auto data = mode_->circleConfig();

    data.angleDelta = r;

    mode_->setCircleConfig(data);
  }
  else if (type == CQCamera3DAddMouseMode::AddType::SPHERE) {
    auto data = mode_->sphereConfig();

    data.angleDelta = r;

    mode_->setSphereConfig(data);
  }

  mode_->updateObject();
}

//---

CQCamera3DMoveOptions::
CQCamera3DMoveOptions(CQCamera3DMoveMouseMode *mode) :
 mode_(mode)
{
}

void
CQCamera3DMoveOptions::
addOptions(CQCamera3DUI *ui)
{
  ui->startGroup("Location");

  dirCombo_ = ui->addLabelEdit("Direction", new QComboBox);
  dirCombo_->addItems(moveDirectionInd.names());

  xEdit_ = ui->addLabelEdit("X", new CQRealSpin);
  yEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
  zEdit_ = ui->addLabelEdit("Z", new CQRealSpin);

  ui->endGroup();

  CPoint3D pos;

  auto *object = mode_->object();

  if (object) {
    auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

    auto *objectData = canvas->getObjectData(object);

    if (objectData) {
      auto bbox = objectData->bbox();

      pos = bbox.getCenter();
    }
  }

  xEdit_->setValue(pos.x);
  yEdit_->setValue(pos.y);
  zEdit_->setValue(pos.z);

  //---

  connectSlots(true);
}

QString
CQCamera3DMoveOptions::
title() const
{
  return "Move";
}

void
CQCamera3DMoveOptions::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, dirCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(dirSlot(int)));
  CQUtil::connectDisconnect(b, xEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xSlot(double)));
  CQUtil::connectDisconnect(b, yEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(ySlot(double)));
  CQUtil::connectDisconnect(b, zEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zSlot(double)));
}

void
CQCamera3DMoveOptions::
dirSlot(int ind)
{
  auto dir = moveDirectionInd.indToType(ind);

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  canvas->setMoveDirection(dir);
}

void
CQCamera3DMoveOptions::
xSlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  auto bbox = objectData->bbox();
  auto dx   = r - bbox.getCenter().x;

  canvas->moveObject(object, CVector3D(dx, 0, 0));
  canvas->update();
}

void
CQCamera3DMoveOptions::
ySlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  auto bbox = objectData->bbox();
  auto dy   = r - bbox.getCenter().y;

  canvas->moveObject(object, CVector3D(0, dy, 0));
  canvas->update();
}

void
CQCamera3DMoveOptions::
zSlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  auto bbox = objectData->bbox();
  auto dz   = r - bbox.getCenter().z;

  canvas->moveObject(object, CVector3D(0, 0, dz));
  canvas->update();
}

//---

CQCamera3DScaleOptions::
CQCamera3DScaleOptions(CQCamera3DScaleMouseMode *mode) :
 mode_(mode)
{
}

void
CQCamera3DScaleOptions::
addOptions(CQCamera3DUI *ui)
{
  ui->startGroup("Location");

  dirCombo_ = ui->addLabelEdit("Direction", new QComboBox);
  dirCombo_->addItems(moveDirectionInd.names());

  xEdit_ = ui->addLabelEdit("X", new CQRealSpin);
  yEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
  zEdit_ = ui->addLabelEdit("Z", new CQRealSpin);

  ui->endGroup();

  CVector3D size;

  auto *object = mode_->object();

  if (object) {
    auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

    auto *objectData = canvas->getObjectData(object);

    if (objectData) {
      auto bbox = objectData->bbox();

      size = bbox.getSize();
    }
  }

  xEdit_->setValue(size.getX());
  yEdit_->setValue(size.getY());
  zEdit_->setValue(size.getZ());

  //---

  connectSlots(true);
}

QString
CQCamera3DScaleOptions::
title() const
{
  return "Scale";
}

void
CQCamera3DScaleOptions::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, dirCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(dirSlot(int)));
  CQUtil::connectDisconnect(b, xEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xSlot(double)));
  CQUtil::connectDisconnect(b, yEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(ySlot(double)));
  CQUtil::connectDisconnect(b, zEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zSlot(double)));
}

void
CQCamera3DScaleOptions::
dirSlot(int ind)
{
  auto dir = moveDirectionInd.indToType(ind);

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  canvas->setMoveDirection(dir);
}

void
CQCamera3DScaleOptions::
xSlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  auto bbox = objectData->bbox();

  auto scale = (r > 0.0 ? r/bbox.getXSize() : 1.0);

  canvas->scaleObject(object, CVector3D(scale, 1, 1));
  canvas->update();
}

void
CQCamera3DScaleOptions::
ySlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  auto bbox = objectData->bbox();

  auto scale = (r > 0.0 ? r/bbox.getXSize() : 1.0);

  canvas->scaleObject(object, CVector3D(1, scale, 1));
  canvas->update();
}

void
CQCamera3DScaleOptions::
zSlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  auto bbox = objectData->bbox();

  auto scale = (r > 0.0 ? r/bbox.getXSize() : 1.0);

  canvas->scaleObject(object, CVector3D(1, 1, scale));
  canvas->update();
}

//---

CQCamera3DRotateOptions::
CQCamera3DRotateOptions(CQCamera3DRotateMouseMode *mode) :
 mode_(mode)
{
}

void
CQCamera3DRotateOptions::
addOptions(CQCamera3DUI *ui)
{
  ui->startGroup("Angle");

  dirCombo_ = ui->addLabelEdit("Direction", new QComboBox);
  dirCombo_->addItems(moveDirectionInd.names());

  xEdit_ = ui->addLabelEdit("X", new CQRealSpin);
  yEdit_ = ui->addLabelEdit("Y", new CQRealSpin);
  zEdit_ = ui->addLabelEdit("Z", new CQRealSpin);

  ui->endGroup();

  xEdit_->setValue(0.0);
  yEdit_->setValue(0.0);
  zEdit_->setValue(0.0);

  //---

  connectSlots(true);
}

QString
CQCamera3DRotateOptions::
title() const
{
  return "Rotate";
}

void
CQCamera3DRotateOptions::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, dirCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(dirSlot(int)));
  CQUtil::connectDisconnect(b, xEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xSlot(double)));
  CQUtil::connectDisconnect(b, yEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(ySlot(double)));
  CQUtil::connectDisconnect(b, zEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zSlot(double)));
}

void
CQCamera3DRotateOptions::
dirSlot(int ind)
{
  auto dir = moveDirectionInd.indToType(ind);

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  canvas->setMoveDirection(dir);
}

void
CQCamera3DRotateOptions::
xSlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  connectSlots(false);

  canvas->rotateObject(object, r, CVector3D(1, 0, 0));
  canvas->update();

  xEdit_->setValue(0.0);

  connectSlots(true);
}

void
CQCamera3DRotateOptions::
ySlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  connectSlots(false);

  canvas->rotateObject(object, r, CVector3D(0, 1, 0));
  canvas->update();

  yEdit_->setValue(0.0);

  connectSlots(true);
}

void
CQCamera3DRotateOptions::
zSlot(double r)
{
  auto *object = mode_->object();
  if (! object) return;

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  connectSlots(false);

  canvas->rotateObject(object, r, CVector3D(0, 0, 1));
  canvas->update();

  zEdit_->setValue(0.0);

  connectSlots(true);
}

//---

CQCamera3DExtrudeOptions::
CQCamera3DExtrudeOptions(CQCamera3DExtrudeMouseMode *mode) :
 mode_(mode)
{
}

void
CQCamera3DExtrudeOptions::
addOptions(CQCamera3DUI *ui)
{
  ui->startGroup("Distance");

  dirCombo_ = ui->addLabelEdit("Direction", new QComboBox);
  dirCombo_->addItems(moveDirectionInd.names());

  distanceEdit_ = ui->addLabelEdit("Distance", new CQRealSpin);

  ui->endGroup();

  //---

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  auto r1 = canvas->extrudeMoveDelta();

  distanceEdit_->setValue(r1);

  //---

  connectSlots(true);
}

QString
CQCamera3DExtrudeOptions::
title() const
{
  return "Extrude";
}

void
CQCamera3DExtrudeOptions::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, dirCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(dirSlot(int)));
  CQUtil::connectDisconnect(b, distanceEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(distanceSlot(double)));
}

void
CQCamera3DExtrudeOptions::
dirSlot(int ind)
{
  auto dir = moveDirectionInd.indToType(ind);

  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  canvas->setMoveDirection(dir);
}

void
CQCamera3DExtrudeOptions::
distanceSlot(double r)
{
  auto *canvas = dynamic_cast<CQCamera3DCanvas *>(mode_->mgr()->widget());

  connectSlots(false);

  auto r1 = canvas->extrudeMoveDelta();

  canvas->extrudeMove(r - r1);

  canvas->setExtrudeMoveDelta(r);

  canvas->update();

  connectSlots(true);
}
