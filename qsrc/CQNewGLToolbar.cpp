#include <CQNewGLToolbar.h>
#include <CQNewGLModel.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>

#include <CQIconButton.h>
#include <CQApp.h>

#include <QStackedWidget>
#include <QLabel>
#include <QHBoxLayout>

#include <svg/camera_svg.h>
#include <svg/model_svg.h>
#include <svg/light_svg.h>
#include <svg/settings_svg.h>

CQNewGLToolbar::
CQNewGLToolbar(CQNewGLModel *app) :
 app_(app)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);

  auto addToolButton = [&](const QString &name, const QString &iconName,
                           const QString &tip, const char *slotName) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);
    button->setFocusPolicy(Qt::NoFocus);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto addCheckButton = [&](const QString &name, const QString &iconName,
                            const QString &tip, const char *slotName) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setCheckable(true);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto addSpacer = [&](int w) {
    auto *spacer = new QFrame;
    spacer->setFixedWidth(w);
    layout->addWidget(spacer);
  };

  //---

  cameraButton_ = addToolButton("camera", "CAMERA", "Camera", SLOT(cameraSlot()));
  modelButton_  = addToolButton("model" , "MODEL" , "Model" , SLOT(modelSlot()));
  lightButton_  = addToolButton("light" , "LIGHT" , "Light" , SLOT(lightSlot()));

  layout->addWidget(cameraButton_);
  layout->addWidget(modelButton_);
  layout->addWidget(lightButton_);

  //---

  addSpacer(24);

  //---

  playButton_  = addToolButton("play" , "PLAY"    , "Play" , SLOT(playSlot()));
  pauseButton_ = addToolButton("pause", "PAUSE"   , "Pause", SLOT(pauseSlot()));
  stepButton_  = addToolButton("step" , "PLAY_ONE", "Step" , SLOT(stepSlot()));

  layout->addWidget(playButton_);
  layout->addWidget(pauseButton_);
  layout->addWidget(stepButton_);

  //---

  layout->addStretch(1);

  //---

  posLabel_ = new QLabel;

  layout->addWidget(posLabel_);

  //---

  layout->addWidget(CQAppInst->createDebugButton());

  //---

  settingsButton_ = addCheckButton("settings", "SETTINGS" , "Settings", SLOT(settingsSlot()));

  layout->addWidget(settingsButton_);
}

void
CQNewGLToolbar::
setPosLabel(const QString &label)
{
  posLabel_->setText(label);
}

void
CQNewGLToolbar::
cameraSlot()
{
  app_->setType(CQNewGLModel::Type::CAMERA);
}

void
CQNewGLToolbar::
modelSlot()
{
  app_->setType(CQNewGLModel::Type::MODEL);
}

void
CQNewGLToolbar::
lightSlot()
{
  app_->setType(CQNewGLModel::Type::LIGHT);
}

void
CQNewGLToolbar::
playSlot()
{
  app_->setTimerRunning(true);
}

void
CQNewGLToolbar::
pauseSlot()
{
  app_->setTimerRunning(false);
}

void
CQNewGLToolbar::
stepSlot()
{
  app_->timerSlot();
}

void
CQNewGLToolbar::
settingsSlot()
{
  auto *button = qobject_cast<CQIconButton *>(sender());

  auto *app     = this->app();
  auto *control = app->controlStack();

  auto geom = app->geometry();

  int w = control->sizeHint().width();

  QRect geom1;

  if (button->isChecked()) {
    geom1 = QRect(geom.x(), geom.y(), geom.width() + w + 6, geom.height());

    control->update();
    control->show();
  }
  else {
    geom1 = QRect(geom.x(), geom.y(), geom.width() - w - 6, geom.height());

    control->hide();
  }

  app->setGeometry(geom1);

  if (button->isChecked())
    control->setFixedWidth(w);
  else {
    control->setMinimumWidth(0);
    control->setMaximumWidth(QWIDGETSIZE_MAX);
  }
}
