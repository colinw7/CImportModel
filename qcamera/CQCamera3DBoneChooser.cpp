#include <CQCamera3DBoneChooser.h>
#include <CQCamera3DApp.h>

#include <CGeomScene3D.h>
#include <CGeomNodeData.h>

#include <CQUtil.h>

CQCamera3DBoneChooser::
CQCamera3DBoneChooser(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("boneChooser");

  connectSlots(true);

  connect(app_, SIGNAL(modelAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);
}

void
CQCamera3DBoneChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQCamera3DBoneChooser::
setObjectInd(int objectInd)
{
  objectInd_ = objectInd;

  needsUpdateSlot();
}

void
CQCamera3DBoneChooser::
setBoneInd(int boneInd)
{
  boneInd_ = boneInd;

  needsUpdateSlot();
}

void
CQCamera3DBoneChooser::
updateWidgets()
{
  if (! needsUpdate_)
    return;

  //---

  connectSlots(false);

  //---

  clear();

  addItem("", QVariant(-1));

  //---

  CGeomObject3D *object = nullptr;

  if (objectInd_ >= 0) {
    auto *scene  = app_->getScene();

    object = scene->getObjectByInd(objectInd_);
  }

  int currentInd = 0;

  if (object) {
    int i = 0;

    for (const auto &pn : object->getNodes()) {
      const auto &nodeData = pn.second;

      auto objectName = QString(" %1").arg(QString::fromStdString(nodeData.name()));

      objectName += QString(" (%1)").arg(nodeData.ind());
      objectName += QString(" (#%1)").arg(nodeData.index());

      addItem(objectName, QVariant(nodeData.ind()));

      if (nodeData.ind() == boneInd_)
        currentInd = i;

      ++i;
    }
  }

  setCurrentIndex(currentInd);

  //---

  connectSlots(true);
}

void
CQCamera3DBoneChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(currentIndexChanged(int)));
}

void
CQCamera3DBoneChooser::
currentIndexChanged(int ind)
{
  boneInd_ = itemData(ind).toInt();

  Q_EMIT boneChanged();
}
