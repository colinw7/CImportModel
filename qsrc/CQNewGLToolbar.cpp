#include <CQNewGLToolbar.h>
#include <CQNewGLModel.h>
#include <CQNewGLControl.h>
#include <CQIconButton.h>

#include <QHBoxLayout>

#include <svg/settings_svg.h>

CQNewGLToolbar::
CQNewGLToolbar(CQNewGLModel *app) :
 app_(app)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QHBoxLayout(this);

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

  settingsButton_ = addCheckButton("settings", "SETTINGS" , "Settings", SLOT(settingsSlot()));

  layout->addStretch(1);

  layout->addWidget(settingsButton_);
}

void
CQNewGLToolbar::
settingsSlot()
{
  auto *button = qobject_cast<CQIconButton *>(sender());

  auto *app     = this->app();
  auto *control = app->control();

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
