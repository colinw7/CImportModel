#include <CQCamera3DObjectChooser.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DGeomObject.h>

#include <CGeomScene3D.h>

#include <CQUtil.h>

CQCamera3DObjectChooser::
CQCamera3DObjectChooser(CQCamera3DApp *app) :
 app_(app)
{
  connectSlots(true);

  connect(app_, SIGNAL(modelAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);

  objectInd_ = -1;
}

void
CQCamera3DObjectChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQCamera3DObjectChooser::
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
CQCamera3DObjectChooser::
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

  auto *scene = app_->getScene();
  auto objects = scene->getObjects();

  int currentInd = 0;

  int ind = 1;

  for (auto *object : objects) {
    //auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

    if (objectInd_ >= 0 && object->getInd() == uint(objectInd)) {
      objectInd_ = object->getInd();
      currentInd = ind;
    }

    auto name     = QString::fromStdString(object->getName());
    auto meshName = QString::fromStdString(object->getMeshName());

    QString label;

    if (meshName != "")
      label = QString("%1 (Mesh: %2, #%3)").arg(name).arg(meshName).arg(object->getInd());
    else
      label = QString("%1 (#%3)").arg(name).arg(object->getInd());

    addItem(label, int(object->getInd()));

    ++ind;
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQCamera3DObjectChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQCamera3DObjectChooser::
currentIndexChanged(int i)
{
  objectInd_ = itemData(i).toInt();

  Q_EMIT objectChanged();
}
