#include <Toolbar.h>
#include <App.h>
#include <Canvas.h>
#include <UI.h>

#include <CQUtil.h>

namespace CQSkybox3D {

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
  modelButton_  = ui.addIconCheckButton("model" , "MODEL" , "Model");
  lightButton_  = ui.addIconCheckButton("light" , "LIGHT" , "Light" );

  cameraButton_->setChecked(true);

  ui.addStretch();

  bufferCheck_ = ui.addCheck("Buffer");
  lightCheck_  = ui.addCheck("Light");

  connectSlots(true);
}

void
Toolbar::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    cameraButton_, SIGNAL(toggled(bool)), this, SLOT(editCameraSlot(bool)));
  CQUtil::connectDisconnect(b,
    modelButton_, SIGNAL(toggled(bool)), this, SLOT(editModelSlot(bool)));
  CQUtil::connectDisconnect(b,
    lightButton_, SIGNAL(toggled(bool)), this, SLOT(editLightSlot(bool)));

  CQUtil::connectDisconnect(b,
    bufferCheck_, SIGNAL(stateChanged(int)), this, SLOT(bufferSlot(int)));
  CQUtil::connectDisconnect(b,
    lightCheck_, SIGNAL(stateChanged(int)), this, SLOT(lightSlot(int)));
}

void
Toolbar::
editCameraSlot(bool state)
{
  if (state) {
    auto *canvas = app_->canvas();

    canvas->setEditType(Canvas::EditType::CAMERA);

    connectSlots(false);

    modelButton_->setChecked(false);
    lightButton_->setChecked(false);

    connectSlots(true);
  }
}

void
Toolbar::
editModelSlot(bool state)
{
  if (state) {
    auto *canvas = app_->canvas();

    canvas->setEditType(Canvas::EditType::MODEL);

    connectSlots(false);

    cameraButton_->setChecked(false);
    lightButton_ ->setChecked(false);

    connectSlots(true);
  }
}

void
Toolbar::
editLightSlot(bool state)
{
  if (state) {
    auto *canvas = app_->canvas();

    canvas->setEditType(Canvas::EditType::LIGHT);

    connectSlots(false);

    cameraButton_->setChecked(false);
    modelButton_ ->setChecked(false);

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

}
