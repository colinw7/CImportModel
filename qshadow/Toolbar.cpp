#include <Toolbar.h>
#include <App.h>
#include <Canvas.h>
#include <Overview.h>
#include <UI.h>

#include <CQUtil.h>

namespace CQShadow3D {

Toolbar::
Toolbar(App *app) :
 app_(app)
{
  setObjectName("toolbar");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(4);

  //---

  UI ui(this, layout);

  cameraButton_ = ui.addIconCheckButton("camera", "CAMERA", "Camera");
  lightButton_  = ui.addIconCheckButton("light" , "LIGHT" , "Light" );

  cameraButton_->setChecked(true);

  ui.addStretch();

  bufferCheck_ = ui.addCheck("Buffer");
  lightCheck_  = ui.addCheck("Light");
  shadowCheck_ = ui.addCheck("Shadow");

  connectSlots(true);
}

void
Toolbar::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    cameraButton_, SIGNAL(toggled(bool)), this, SLOT(editCameraSlot(bool)));
  CQUtil::connectDisconnect(b,
    lightButton_, SIGNAL(toggled(bool)), this, SLOT(editLightSlot(bool)));

  CQUtil::connectDisconnect(b,
    bufferCheck_, SIGNAL(stateChanged(int)), this, SLOT(bufferSlot(int)));
  CQUtil::connectDisconnect(b,
    lightCheck_, SIGNAL(stateChanged(int)), this, SLOT(lightSlot(int)));
  CQUtil::connectDisconnect(b,
    shadowCheck_, SIGNAL(stateChanged(int)), this, SLOT(shadowSlot(int)));
}

void
Toolbar::
editCameraSlot(bool state)
{
  if (state) {
    auto *canvas   = app_->canvas();
    auto *overview = app_->overview();

    canvas  ->setEditType(Canvas  ::EditType::CAMERA);
    overview->setEditType(Overview::EditType::CAMERA);

    connectSlots(false);

    lightButton_->setChecked(false);

    connectSlots(true);
  }
}

void
Toolbar::
editLightSlot(bool state)
{
  if (state) {
    auto *canvas   = app_->canvas();
    auto *overview = app_->overview();

    canvas  ->setEditType(Canvas  ::EditType::LIGHT);
    overview->setEditType(Overview::EditType::LIGHT);

    connectSlots(false);

    cameraButton_->setChecked(false);

    connectSlots(true);
  }
}

void
Toolbar::
bufferSlot(int state)
{
  auto *canvas = app_->canvas();

  canvas->setTextureBuffer(state);

  canvas->update();
}

void
Toolbar::
lightSlot(int state)
{
  auto *canvas = app_->canvas();

  canvas->setLightBuffer(state);

  canvas->update();
}

void
Toolbar::
shadowSlot(int state)
{
  auto *canvas = app_->canvas();

  canvas->setShadowed(state);

  canvas->update();
}

}
