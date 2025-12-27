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
  auto animNames = app_->getAnimNames(tmin_, tmax_);

  int ind = 0;

  for (const auto &animName : animNames) {
    if (animName == name) {
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

  auto animNames = app_->getAnimNames(tmin_, tmax_);

  for (const auto &animName : animNames) {
    addItem(animName, QVariant(ind++));
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

  auto animNames = app_->getAnimNames(tmin_, tmax_);

  int ind1 = 1;

  for (const auto &animName : animNames) {
    if (ind == ind1) {
      animName_ = animName;
      break;
    }

    ++ind1;
  }

  Q_EMIT animChanged();
}
