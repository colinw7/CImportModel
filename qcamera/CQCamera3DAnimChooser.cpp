#include <CQCamera3DAnimChooser.h>
#include <CQCamera3DApp.h>

#include <CQUtil.h>

CQCamera3DAnimChooser::
CQCamera3DAnimChooser(CQCamera3DApp *app) :
 app_(app)
{
  connectSlots(true);

  connect(app_, SIGNAL(modelAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);
}

void
CQCamera3DAnimChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQCamera3DAnimChooser::
setAnimName(const QString &name)
{
  auto animNames = app_->getAnimNames();

  int ind = 0;

  for (const auto &animName : animNames) {
    if (animName.name == name) {
      tmin_ = animName.tmin;
      tmax_ = animName.tmax;

      setCurrentIndex(ind + 1);

      return;
    }

    ++ind;
  }

  setCurrentIndex(0);
}

void
CQCamera3DAnimChooser::
updateWidgets()
{
  if (! needsUpdate_)
    return;

  //---

  connectSlots(false);

  //---

  int currentInd = currentIndex();

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  auto animNames = app_->getAnimNames();

  for (const auto &animName : animNames) {
    addItem(animName.name, QVariant(ind++));
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQCamera3DAnimChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQCamera3DAnimChooser::
currentIndexChanged(int ind)
{
  animName_ = "";
  tmin_     = 0.0;
  tmax_     = 0.0;

  auto animNames = app_->getAnimNames();

  int ind1 = 1;

  for (const auto &animName : animNames) {
    if (ind == ind1) {
      tmin_ = animName.tmin;
      tmax_ = animName.tmax;

      animName_ = animName.name;

      break;
    }

    ++ind1;
  }

  Q_EMIT animChanged();
}
