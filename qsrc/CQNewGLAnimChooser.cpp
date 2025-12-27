#include <CQNewGLAnimChooser.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQUtil.h>

CQNewGLAnimChooser::
CQNewGLAnimChooser(CQNewGLControl *control) :
 control_(control)
{
  connectSlots(true);

  auto *canvas = control_->canvas();
  auto *app    = canvas->app();

  connect(app, SIGNAL(modelAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);
}

void
CQNewGLAnimChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQNewGLAnimChooser::
setAnimName(const QString &name)
{
  auto *canvas = control_->canvas();

  auto animNames = canvas->getAnimNames(tmin_, tmax_);

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
CQNewGLAnimChooser::
updateWidgets()
{
  if (! needsUpdate_)
    return;

  //---

  connectSlots(false);

  //---

  int currentInd = currentIndex();

  auto *canvas = control_->canvas();

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  auto animNames = canvas->getAnimNames(tmin_, tmax_);

  for (const auto &animName : animNames) {
    addItem(animName, QVariant(ind++));
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQNewGLAnimChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQNewGLAnimChooser::
currentIndexChanged(int ind)
{
  auto *canvas = control_->canvas();

  animName_ = "";

  auto animNames = canvas->getAnimNames(tmin_, tmax_);

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
