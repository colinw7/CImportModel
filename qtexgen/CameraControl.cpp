#include <CameraControl.h>
#include <MaterialView.h>
#include <Camera.h>
#include <App.h>
#include <UI.h>

#include <CQRealSpin.h>
#include <CQUtil.h>

#include <QVBoxLayout>

namespace CQTextureGen {

CameraControl::
CameraControl(App *app) :
 app_(app)
{
  setObjectName("cameraControl");

//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  ui.startGroup("Position");

  xPosEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  yPosEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  zPosEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Origin");

  xOriginEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  yOriginEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  zOriginEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  fovEdit_  = ui.addLabelEdit("Fov" , new CQRealSpin);
  nearEdit_ = ui.addLabelEdit("Near", new CQRealSpin);
  farEdit_  = ui.addLabelEdit("Far" , new CQRealSpin);

  ui.addStretch();

  connectSlots(true);

  updateWidgets();
}

void
CameraControl::
updateWidgets()
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  xPosEdit_->setValue(camera->position().x());
  yPosEdit_->setValue(camera->position().y());
  zPosEdit_->setValue(camera->position().z());

  xOriginEdit_->setValue(camera->origin().x());
  yOriginEdit_->setValue(camera->origin().y());
  zOriginEdit_->setValue(camera->origin().z());

  fovEdit_ ->setValue(camera->fov ());
  nearEdit_->setValue(camera->near());
  farEdit_ ->setValue(camera->far ());

  connectSlots(true);
}

void
CameraControl::
connectSlots(bool b)
{
  auto *camera  = app_->materialView()->camera();
  auto *camera1 = dynamic_cast<Camera *>(camera);

  CQUtil::connectDisconnect(b, camera1, SIGNAL(stateChanged()),
                            this, SLOT(updateWidgets()));

  CQUtil::connectDisconnect(b, xPosEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xPosSlot(double)));
  CQUtil::connectDisconnect(b, yPosEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(yPosSlot(double)));
  CQUtil::connectDisconnect(b, zPosEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zPosSlot(double)));

  CQUtil::connectDisconnect(b, xOriginEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xOriginSlot(double)));
  CQUtil::connectDisconnect(b, yOriginEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(yOriginSlot(double)));
  CQUtil::connectDisconnect(b, zOriginEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zOriginSlot(double)));

  CQUtil::connectDisconnect(b, fovEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(fovSlot(double)));
  CQUtil::connectDisconnect(b, nearEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(nearSlot(double)));
  CQUtil::connectDisconnect(b, farEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(farSlot(double)));
}

void
CameraControl::
xPosSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  auto pos = camera->position();

  pos.setX(r);

  camera->setPosition(pos);

  connectSlots(true);
}

void
CameraControl::
yPosSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  auto pos = camera->position();

  pos.setY(r);

  camera->setPosition(pos);

  connectSlots(true);
}

void
CameraControl::
zPosSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  auto pos = camera->position();

  pos.setZ(r);

  camera->setPosition(pos);

  connectSlots(true);
}

void
CameraControl::
xOriginSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  auto pos = camera->origin();

  pos.setX(r);

  camera->setOrigin(pos);

  connectSlots(true);
}

void
CameraControl::
yOriginSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  auto pos = camera->origin();

  pos.setY(r);

  camera->setOrigin(pos);

  connectSlots(true);
}

void
CameraControl::
zOriginSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  auto pos = camera->origin();

  pos.setZ(r);

  camera->setOrigin(pos);

  connectSlots(true);
}

void
CameraControl::
fovSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  camera->setFov(r);

  connectSlots(true);
}

void
CameraControl::
nearSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  camera->setNear(r);

  connectSlots(true);
}

void
CameraControl::
farSlot(double r)
{
  connectSlots(false);

  auto *camera = app_->materialView()->camera();

  camera->setFar(r);

  connectSlots(true);
}

}
