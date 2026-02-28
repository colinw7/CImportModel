#include <CQCamera3DAnimObjectChooser.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DGeomObject.h>

#include <CQUtil.h>

CQCamera3DAnimObjectChooser::
CQCamera3DAnimObjectChooser(CQCamera3DApp *app) :
 app_(app)
{
  connectSlots(true);

  connect(app_, SIGNAL(modelAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);

  objectInd_ = -1;
}

void
CQCamera3DAnimObjectChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQCamera3DAnimObjectChooser::
setObject(CQCamera3DGeomObject *object)
{
  objectInd_ = (object ? object->getInd() : -1);

  int n = count();

  for (int i = 0; i < n; ++i) {
    int ind = itemData(i).toInt();

    if (ind == objectInd_) {
      setCurrentIndex(i);
      return;
    }
  }

  setCurrentIndex(0);
}

void
CQCamera3DAnimObjectChooser::
updateWidgets()
{
  if (! needsUpdate_)
    return;

  //---

  connectSlots(false);

  //---

  int objectInd = objectInd_;

  objectInd_ = -1;

  clear();

  addItem("", QVariant(-1));

  auto animObjects = app_->getAnimObjects();

  int currentInd = 0;

  int ind = 1;

  for (auto *animObject : animObjects) {
    if (objectInd_ >= 0 && animObject->getInd() == uint(objectInd)) {
      objectInd_ = animObject->getInd();
      currentInd = ind;
    }

    auto name = QString::fromStdString(animObject->getName());

    addItem(name, int(animObject->getInd()));

    ++ind;
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQCamera3DAnimObjectChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQCamera3DAnimObjectChooser::
currentIndexChanged(int i)
{
  objectInd_ = itemData(i).toInt();

  Q_EMIT objectChanged();
}
