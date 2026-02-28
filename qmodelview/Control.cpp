#include <Control.h>
#include <Canvas.h>
#include <Camera.h>
#include <App.h>
#include <UI.h>

#include <CQRealSpin.h>

#include <QCheckBox>

namespace CQModel3DView {

Control::
Control(App *app) :
 app_(app)
{
  setObjectName("control");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  UI ui(this, layout);

  //---

  disableRollCheck_ = ui.addLabelEdit("Disable Roll", new QCheckBox);

  ui.startGroup("Clamp Pitch");

  clampPitchCheck_ = ui.addLabelEdit("Enabled", new QCheckBox);
  minPitchEdit_    = ui.addLabelEdit("Min"    , new CQRealSpin);
  maxPitchEdit_    = ui.addLabelEdit("Max"    , new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Clamp Yaw");

  clampYawCheck_ = ui.addLabelEdit("Yaw", new QCheckBox);
  minYawEdit_    = ui.addLabelEdit("Min", new CQRealSpin);
  maxYawEdit_    = ui.addLabelEdit("Max", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Clamp Roll");

  clampRollCheck_ = ui.addLabelEdit("Roll", new QCheckBox);
  minRollEdit_    = ui.addLabelEdit("Min" , new CQRealSpin);
  maxRollEdit_    = ui.addLabelEdit("Max" , new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Origin");

  xOriginEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  yOriginEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  zOriginEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Position");

  xPosEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  yPosEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  zPosEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  distanceEdit_ = ui.addLabelEdit("Distance", new CQRealSpin);

  ui.startGroup("Angles");

  pitchEdit_ = ui.addLabelEdit("Pitch", new CQRealSpin);
  yawEdit_   = ui.addLabelEdit("Yaw"  , new CQRealSpin);
  rollEdit_  = ui.addLabelEdit("Roll" , new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Z");

  nearEdit_ = ui.addLabelEdit("Near", new CQRealSpin);
  farEdit_  = ui.addLabelEdit("Far" , new CQRealSpin);
  fovEdit_  = ui.addLabelEdit("FOV" , new CQRealSpin);

  ui.endGroup();

  ui.addStretch();

  //---

  connectSlots(true);

  updateWidgets();

  connect(camera(), SIGNAL(stateChangedSignal()), this, SLOT(updateWidgets()));
}

void
Control::
connectSlots(bool b)
{
  auto connectCheckBox = [&](QCheckBox *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(stateChanged(int)), this, slotName);
  };

  auto connectRealSpin = [&](CQRealSpin *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(realValueChanged(double)), this, slotName);
  };

  //---

  connectCheckBox(disableRollCheck_, SLOT(disableRollSlot(int)));

  connectCheckBox(clampPitchCheck_, SLOT(clampPitchSlot(int)));
  connectRealSpin(minPitchEdit_   , SLOT(minPitchSlot(double)));
  connectRealSpin(maxPitchEdit_   , SLOT(maxPitchSlot(double)));

  connectCheckBox(clampYawCheck_, SLOT(clampYawSlot(int)));
  connectRealSpin(minYawEdit_   , SLOT(minYawSlot(double)));
  connectRealSpin(maxYawEdit_   , SLOT(maxYawSlot(double)));

  connectCheckBox(clampRollCheck_, SLOT(clampRollSlot(int)));
  connectRealSpin(minRollEdit_   , SLOT(minRollSlot(double)));
  connectRealSpin(maxRollEdit_   , SLOT(maxRollSlot(double)));

  connectRealSpin(xOriginEdit_, SLOT(xOriginSlot(double)));
  connectRealSpin(yOriginEdit_, SLOT(yOriginSlot(double)));
  connectRealSpin(zOriginEdit_, SLOT(zOriginSlot(double)));

  connectRealSpin(xPosEdit_, SLOT(xPosSlot(double)));
  connectRealSpin(yPosEdit_, SLOT(yPosSlot(double)));
  connectRealSpin(zPosEdit_, SLOT(zPosSlot(double)));

  connectRealSpin(distanceEdit_, SLOT(distanceSlot(double)));

  connectRealSpin(pitchEdit_, SLOT(pitchSlot(double)));
  connectRealSpin(yawEdit_  , SLOT(yawSlot(double)));
  connectRealSpin(rollEdit_ , SLOT(rollSlot(double)));

  connectRealSpin(nearEdit_, SLOT(nearSlot(double)));
  connectRealSpin(farEdit_ , SLOT(farSlot(double)));

  connectRealSpin(fovEdit_, SLOT(fovSlot(double)));
}

void
Control::
updateWidgets()
{
  auto *camera = this->camera();

  connectSlots(false);

  disableRollCheck_->setChecked(camera->isDisableRoll());

  clampPitchCheck_->setChecked(camera->isClampPitch());
  minPitchEdit_   ->setValue(camera->minPitch());
  maxPitchEdit_   ->setValue(camera->maxPitch());

  clampYawCheck_->setChecked(camera->isClampYaw());
  minYawEdit_   ->setValue  (camera->minYaw());
  maxYawEdit_   ->setValue  (camera->maxYaw());

  clampRollCheck_->setChecked(camera->isClampRoll());
  minRollEdit_   ->setValue  (camera->minRoll());
  maxRollEdit_   ->setValue  (camera->maxRoll());

  xOriginEdit_->setValue(camera->origin().x());
  yOriginEdit_->setValue(camera->origin().y());
  zOriginEdit_->setValue(camera->origin().z());

  xPosEdit_->setValue(camera->position().x());
  yPosEdit_->setValue(camera->position().y());
  zPosEdit_->setValue(camera->position().z());

  distanceEdit_->setValue(camera->distance());

  pitchEdit_->setValue(camera->pitch());
  yawEdit_  ->setValue(camera->yaw());
  rollEdit_ ->setValue(camera->roll());

  nearEdit_->setValue(camera->near());
  farEdit_ ->setValue(camera->far());

  fovEdit_->setValue(camera->fov());

  connectSlots(true);
}

void
Control::
disableRollSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setDisableRoll(state);
}

void
Control::
clampPitchSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setClampPitch(state);
}

void
Control::
minPitchSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMinPitch(r);
}

void
Control::
maxPitchSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMaxPitch(r);
}

void
Control::
clampYawSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setClampYaw(state);
}

void
Control::
minYawSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMinYaw(r);
}

void
Control::
maxYawSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMaxYaw(r);
}

void
Control::
clampRollSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setClampRoll(state);
}

void
Control::
minRollSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMinRoll(r);
}

void
Control::
maxRollSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMaxRoll(r);
}

void
Control::
xOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = camera()->origin(); o.setX(r);

  camera()->setOrigin(o);
}

void
Control::
yOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = camera()->origin(); o.setY(r);

  camera()->setOrigin(o);
}

void
Control::
zOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = camera()->origin(); o.setY(r);

  camera()->setOrigin(o);
}

void
Control::
xPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setX(r);

  camera()->setPosition(p);
}

void
Control::
yPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setY(r);

  camera()->setPosition(p);
}

void
Control::
zPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setY(r);

  camera()->setPosition(p);
}

void
Control::
distanceSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setDistance(r);
}

void
Control::
pitchSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setPitch(r);
}

void
Control::
yawSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setYaw(r);
}

void
Control::
rollSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setRoll(r);
}

void
Control::
nearSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setNear(r);
}

void
Control::
farSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setFar(r);
}

void
Control::
fovSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setFov(r);
}

Camera *
Control::
camera() const
{
  return app_->canvas()->camera();
}

}
