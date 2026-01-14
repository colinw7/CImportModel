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

  selectButton_ = addCheckButton("select", "SELECT", "Select");
  cursorButton_ = addCheckButton("cursor", "CURSOR", "Cursor");
  moveButton_   = addCheckButton("move"  , "MOVE"  , "Move"  );
  rotateButton_ = addCheckButton("rotate", "ROTATE", "Rotate");
  scaleButton_  = addCheckButton("scale" , "SCALE" , "Scale" );

  selectButton_->setChecked(true);

  layout->addStretch(1);

  connectSlots(true);
}

void
CQCamera3DSidebar::
setView(const View &view)
{
  view_ = view;

  updateEditType();
}

void
CQCamera3DSidebar::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    selectButton_, SIGNAL(toggled(bool)), this, SLOT(selectSlot(bool)));
  CQUtil::connectDisconnect(b,
    cursorButton_, SIGNAL(toggled(bool)), this, SLOT(cursorSlot(bool)));
  CQUtil::connectDisconnect(b,
    moveButton_, SIGNAL(toggled(bool)), this, SLOT(moveSlot(bool)));
  CQUtil::connectDisconnect(b,
    rotateButton_, SIGNAL(toggled(bool)), this, SLOT(rotateSlot(bool)));
  CQUtil::connectDisconnect(b,
    scaleButton_, SIGNAL(toggled(bool)), this, SLOT(scaleSlot(bool)));
}

void
CQCamera3DSidebar::
selectSlot(bool state)
{
  connectSlots(false);

  if (state) {
    for (auto *button : checkButtons_)
      button->setChecked(false);

    selectButton_->setChecked(true);
  }
  else
    selectButton_->setChecked(true);

  editType_ = CQCamera3DEditType::SELECT;

  updateEditType();

  connectSlots(true);
}

void
CQCamera3DSidebar::
cursorSlot(bool state)
{
  connectSlots(false);

  for (auto *button : checkButtons_)
    button->setChecked(false);

  if (state)
    cursorButton_->setChecked(true);
  else
    selectButton_->setChecked(true);

  editType_ = (state ? CQCamera3DEditType::CURSOR : CQCamera3DEditType::SELECT);

  updateEditType();

  connectSlots(true);
}

void
CQCamera3DSidebar::
moveSlot(bool state)
{
  connectSlots(false);

  for (auto *button : checkButtons_)
    button->setChecked(false);

  if (state)
    moveButton_->setChecked(true);
  else
    selectButton_->setChecked(true);

  editType_ = (state ? CQCamera3DEditType::MOVE : CQCamera3DEditType::SELECT);

  updateEditType();

  connectSlots(true);
}

void
CQCamera3DSidebar::
rotateSlot(bool state)
{
  connectSlots(false);

  for (auto *button : checkButtons_)
    button->setChecked(false);

  if (state)
    rotateButton_->setChecked(true);
  else
    selectButton_->setChecked(true);

  editType_ = (state ? CQCamera3DEditType::ROTATE : CQCamera3DEditType::SELECT);

  updateEditType();

  connectSlots(true);
}

void
CQCamera3DSidebar::
scaleSlot(bool state)
{
  connectSlots(false);

  for (auto *button : checkButtons_)
    button->setChecked(false);

  if (state)
    scaleButton_->setChecked(true);
  else
    selectButton_->setChecked(true);

  editType_ = (state ? CQCamera3DEditType::SCALE : CQCamera3DEditType::SELECT);

  connectSlots(true);
}

void
CQCamera3DSidebar::
updateEditType()
{
  if      (view_ == View::CANVAS)
    app_->canvas()->setEditType(editType_);
  else if (view_ == View::OVERVIEW)
    app_->overview()->setEditType(editType_);
}
