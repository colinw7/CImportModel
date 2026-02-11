#include <CQCamera3DSidebar.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DOverview.h>

#include <CQIconButton.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQCamera3DSidebar::
CQCamera3DSidebar(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("toolbar");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(4);

  auto addToolButton = [&](const QString &name, const QString &iconName, const QString &tip) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);

    layout->addWidget(button);

    return button;
  };

  auto addCheckButton = [&](const QString &name, const QString &iconName, const QString &tip) {
    auto *button = addToolButton(name, iconName, tip);

    button->setCheckable(true);

    checkButtons_.push_back(button);

    return button;
  };

  // edit mode buttons
  selectButton_ = addCheckButton("select", "SELECT", "Select");
  cursorButton_ = addCheckButton("cursor", "CURSOR", "Cursor");
  cameraButton_ = addCheckButton("camera", "CAMERA", "Camera");
  lightButton_  = addCheckButton("light" , "LIGHT" , "Light" );
  moveButton_   = addCheckButton("move"  , "MOVE"  , "Move"  );
  rotateButton_ = addCheckButton("rotate", "ROTATE", "Rotate");
  scaleButton_  = addCheckButton("scale" , "SCALE" , "Scale" );

  // function buttons
  extrudeButton_ = addToolButton("extrude", "EXTRUDE" , "Extrude" );
  loopCutButton_ = addToolButton("loopcut", "LOOP_CUT", "Loop Cut");

  selectButton_->setChecked(true);

  layout->addStretch(1);

  connectSlots(true);
}

void
CQCamera3DSidebar::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    app_, SIGNAL(viewTypeChanged()), this, SLOT(viewTypeSlot()));

  CQUtil::connectDisconnect(b,
    app_->canvas(), SIGNAL(editTypeChanged()), this, SLOT(updateButtonState()));

  CQUtil::connectDisconnect(b,
    selectButton_, SIGNAL(toggled(bool)), this, SLOT(selectSlot(bool)));
  CQUtil::connectDisconnect(b,
    cursorButton_, SIGNAL(toggled(bool)), this, SLOT(cursorSlot(bool)));
  CQUtil::connectDisconnect(b,
    cameraButton_, SIGNAL(toggled(bool)), this, SLOT(cameraSlot(bool)));
  CQUtil::connectDisconnect(b,
    lightButton_, SIGNAL(toggled(bool)), this, SLOT(lightSlot(bool)));
  CQUtil::connectDisconnect(b,
    moveButton_, SIGNAL(toggled(bool)), this, SLOT(moveSlot(bool)));
  CQUtil::connectDisconnect(b,
    rotateButton_, SIGNAL(toggled(bool)), this, SLOT(rotateSlot(bool)));
  CQUtil::connectDisconnect(b,
    scaleButton_, SIGNAL(toggled(bool)), this, SLOT(scaleSlot(bool)));

  CQUtil::connectDisconnect(b,
    extrudeButton_, SIGNAL(clicked()), this, SLOT(extrudeSlot()));
  CQUtil::connectDisconnect(b,
    loopCutButton_, SIGNAL(clicked()), this, SLOT(loopCutSlot()));
}

void
CQCamera3DSidebar::
viewTypeSlot()
{
  viewType_ = app_->viewType();

  cursorButton_->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);

  cameraButton_->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);
  lightButton_ ->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);

  moveButton_  ->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);
  rotateButton_->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);
  scaleButton_ ->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);

  extrudeButton_->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);
  loopCutButton_->setVisible(viewType_ == ViewType::MODEL ||
                            viewType_ == ViewType::OVERVIEW);

  updateEditType();
}

void
CQCamera3DSidebar::
selectSlot(bool)
{
  editType_ = EditType::SELECT;

  updateEditType();

  updateButtonState();
}

void
CQCamera3DSidebar::
cursorSlot(bool state)
{
  editType_ = (state ? EditType::CURSOR : EditType::SELECT);

  updateEditType();

  updateButtonState();
}

void
CQCamera3DSidebar::
cameraSlot(bool state)
{
  editType_ = (state ? EditType::CAMERA : EditType::SELECT);

  updateEditType();

  updateButtonState();
}

void
CQCamera3DSidebar::
lightSlot(bool state)
{
  editType_ = (state ? EditType::LIGHT : EditType::SELECT);

  updateEditType();

  updateButtonState();
}

void
CQCamera3DSidebar::
moveSlot(bool state)
{
  editType_ = (state ? EditType::MOVE : EditType::SELECT);

  updateEditType();

  updateButtonState();
}

void
CQCamera3DSidebar::
rotateSlot(bool state)
{
  editType_ = (state ? EditType::ROTATE : EditType::SELECT);

  updateEditType();

  updateButtonState();
}

void
CQCamera3DSidebar::
scaleSlot(bool state)
{
  editType_ = (state ? EditType::SCALE : EditType::SELECT);

  updateEditType();

  updateButtonState();
}

void
CQCamera3DSidebar::
updateButtonState()
{
  connectSlots(false);

  if      (viewType_ == ViewType::MODEL)
    editType_ = app_->canvas()->editType();
  else if (viewType_ == ViewType::OVERVIEW)
    editType_ = app_->overview()->editType();

  selectButton_->setChecked(editType_ == EditType::SELECT);
  cursorButton_->setChecked(editType_ == EditType::CURSOR);
  cameraButton_->setChecked(editType_ == EditType::CAMERA);
  lightButton_ ->setChecked(editType_ == EditType::LIGHT);
  moveButton_  ->setChecked(editType_ == EditType::MOVE);
  rotateButton_->setChecked(editType_ == EditType::ROTATE);
  scaleButton_ ->setChecked(editType_ == EditType::SCALE);

  connectSlots(true);
}

void
CQCamera3DSidebar::
updateEditType()
{
  connectSlots(false);

  if      (viewType_ == ViewType::MODEL)
    app_->canvas()->setEditType(editType_);
  else if (viewType_ == ViewType::OVERVIEW)
    app_->overview()->setEditType(editType_);

  connectSlots(true);
}

void
CQCamera3DSidebar::
extrudeSlot()
{
  // extrude
  app_->canvas()->extrude();

  // move mode
  app_->canvas()->extrudeMode();
}

void
CQCamera3DSidebar::
loopCutSlot()
{
  // loop cut
  app_->canvas()->loopCut();

  // TODO: mode
}
