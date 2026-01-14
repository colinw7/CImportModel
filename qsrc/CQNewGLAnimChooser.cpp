#include <CQNewGLAnimChooser.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>

#include <CQUtil.h>

CQNewGLAnimChooser::
CQNewGLAnimChooser(CQNewGLModel *app) :
 app_(app)
{
  connectSlots(true);

  connect(app_, SIGNAL(modelAdded()), this, SLOT(needsUpdateSlot()));

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
  auto *canvas = app_->canvas();

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

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  auto *canvas = app_->canvas();

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
  animName_ = "";

  auto *canvas = app_->canvas();

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
