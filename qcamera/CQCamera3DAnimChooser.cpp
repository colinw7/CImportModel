#include <CQCamera3DAnimChooser.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DGeomObject.h>

#include <CGeomScene3D.h>

#include <CQUtil.h>

CQCamera3DAnimChooser::
CQCamera3DAnimChooser(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("animChooser");

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
setObjectInd(int objectInd)
{
  objectInd_ = objectInd;

  needsUpdateSlot();
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

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  AnimData animData;
  getAnimData(animData);

  for (const auto &animName1 : animData.names) {
    addItem(animName1, QVariant(ind++));
  }

  //---

  int currentIndex = 0;

  ind = 0;

  for (const auto &animName1 : animData.names) {
    if (animName1 == animData.name) {
      animData_ = animData;

      currentIndex = ind + 1;

      break;
    }

    ++ind;
  }

  setCurrentIndex(currentIndex);

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
  AnimData animData;
  getAnimData(animData);

  int ind1 = 1;

  QString animName;

  for (const auto &animName1 : animData.names) {
    if (ind == ind1) {
      animName = animName1;
      break;
    }

    ++ind1;
  }

  auto *object = getObject();

  if (object) {
    object->setAnimName(animName.toStdString());

    updateWidgets();
  }

  app_->invalidateNodeMatrices();

  Q_EMIT animChanged();
}

CGeomObject3D *
CQCamera3DAnimChooser::
getObject() const
{
  auto *scene = app_->getScene();

  return scene->getObjectByInd(objectInd_);
}

void
CQCamera3DAnimChooser::
getAnimData(AnimData &animData) const
{
  auto *object = getObject();

  if (object) {
    auto *object1 = dynamic_cast<CQCamera3DGeomObject *>(object);

    animData.names = object1->getAnimNames();
    animData.name  = QString::fromStdString(object1->animName());

    object->getAnimationTranslationRange(object1->animName(), animData.tmin, animData.tmax);

    animData.timeStep = object1->animTimeStep();
  }
  else {
    animData.names = app_->getAnimNames();
  }
}
