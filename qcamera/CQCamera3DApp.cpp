#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DUVMap.h>
#include <CQCamera3DTextures.h>
#include <CQCamera3DBones.h>
#include <CQCamera3DControl.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DGeomObject.h>
#include <CQCamera3DLight.h>

#include <CGeometry3D.h>
#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CFile.h>

#include <CQTabSplit.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QTimer>

#include <svg/light_svg.h>
#include <svg/play_svg.h>
#include <svg/pause_svg.h>
#include <svg/play_one_svg.h>

#include <set>
#include <iostream>

#define Q(x) #x
#define QUOTE(x) Q(x)

class CQCamera3DGeomFactory : public CGeometryFactory {
 public:
  CQCamera3DGeomFactory() { }
 ~CQCamera3DGeomFactory() override { }

  CGeomObject3D *createObject3D(CGeomScene3D *pscene, const std::string &name) const override {
    return new CQCamera3DGeomObject(pscene, name);
  }

  CGeomLight3D *createLight3D(CGeomScene3D *pscene, const std::string &name) const override {
    return new CQCamera3DLight(pscene, name);
  }
};

//---

CQCamera3DApp::
CQCamera3DApp()
{
  setObjectName("app");

  buildDir_ = QUOTE(BUILD_DIR);

  //---

  CGeometryInst->setFactory(new CQCamera3DGeomFactory);

  scene_ = CGeometryInst->createScene3D();

  //---

  auto *layout = new QVBoxLayout(this);

  auto *frame = CQUtil::makeWidget<CQTabSplit>("topTab");

  layout->addWidget(frame);

  tab_ = CQUtil::makeWidget<CQTabSplit>("centerTab");

  tab_->setState(CQTabSplit::State::TAB);

  canvas_   = new CQCamera3DCanvas(this);
  overview_ = new CQCamera3DOverview(this);
  uvMap_    = new CQCamera3DUVMap(this);
  textures_ = new CQCamera3DTextures(this);
  bones_    = new CQCamera3DBones(this);

  tab_->addWidget(canvas_  , "3D"      );
  tab_->addWidget(overview_, "Overview");
  tab_->addWidget(uvMap_   , "UV"      );
  tab_->addWidget(textures_, "Textures");
  tab_->addWidget(bones_   , "Bones"   );

  control_  = new CQCamera3DControl(this);

  frame->addWidget(tab_    , "Model"  );
  frame->addWidget(control_, "Control");

  status_ = new CQCamera3DStatus(this);

  layout->addWidget(status_);

  //---

  timer_ = new QTimer;

  connect(timer_, &QTimer::timeout, this, &CQCamera3DApp::timerSlot);

  //---

  connectSlots(true);
}

void
CQCamera3DApp::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, tab_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(tabSlot(int)));

  CQUtil::connectDisconnect(b, canvas_, SIGNAL(textureAdded()),
                            this, SIGNAL(textureAdded()));
}

void
CQCamera3DApp::
tabSlot(int i)
{
  control_->setCurrentControl(i);
}

void
CQCamera3DApp::
setCurrentView(int i)
{
  connectSlots(false);

  tab_->setCurrentIndex(i);

  connectSlots(true);
}

//---

void
CQCamera3DApp::
setTimerRunning(bool b)
{
  if (b != timerRunning_) {
    timerRunning_ = b;

    if (timerRunning_) {
      startTimer();

      timerSlot();
    }
    else
      stopTimer();
  }
}

void
CQCamera3DApp::
startTimer()
{
  timer_->start(100);
}

void
CQCamera3DApp::
stopTimer()
{
  timer_->stop();
}

void
CQCamera3DApp::
timerSlot()
{
  ++ticks_;

  Q_EMIT timerStep();
}

//---

void
CQCamera3DApp::
setCurrentBoneObject(int i)
{
  currentBoneObject_ = i;

  bones_->update();

  Q_EMIT boneNodeChanged();
}

void
CQCamera3DApp::
setCurrentBoneNode(int i)
{
  currentBoneNode_ = i;

  bones_->update();

  Q_EMIT boneNodeChanged();
}

//---

void
CQCamera3DApp::
setAnimName(const QString &s)
{
  animName_ = s;

  Q_EMIT animNameChanged();
}

void
CQCamera3DApp::
setAnimTime(double r)
{
  animTime_ = r;

  Q_EMIT animTimeChanged();
}

//---

bool
CQCamera3DApp::
loadModel(const QString &fileName, CGeom3DType format, LoadData &loadData)
{
  static uint modeInd;

  auto modelName = QString("Model.%1").arg(++modeInd);

  auto *im = CImportBase::createModel(format, modelName.toStdString());

  if (! im) {
    std::cerr << "Invalid format\n";
    return false;
  }

  im->setInvertX(loadData.invertX);
  im->setInvertY(loadData.invertY);
  im->setInvertZ(loadData.invertZ);

  im->setSwapXY(loadData.swapXY);
  im->setSwapYZ(loadData.swapYZ);
  im->setSwapZX(loadData.swapZX);

  CFile file(fileName.toStdString());

  if (! im->read(file)) {
    delete im;
    std::cerr << "Failed to read model '" << fileName.toStdString() << "'\n";
    return false;
  }

  auto *scene = im->releaseScene();

  delete im;

  uint numTop = 0;

  for (auto *object : scene->getObjects()) {
    if (! object->parent())
      ++numTop;
  }

  if (numTop > 1) {
    auto *parentObj = CGeometryInst->createObject3D(scene_, modelName.toStdString());

    scene_->addObject(parentObj);

    //loadData.objects.push_back(parentObj);

    for (auto *object : scene->getObjects()) {
      scene_->addObject(object);

      if (! object->parent())
        parentObj->addChild(object);

      loadData.objects.push_back(object);
    }
  }
  else {
    for (auto *object : scene->getObjects()) {
      scene_->addObject(object);

      loadData.objects.push_back(object);
    }
  }

  for (auto *material : scene->getMaterials()) {
    scene_->addMaterial(material);
  }

  Q_EMIT modelAdded();

  return true;
}

//---

std::vector<QString>
CQCamera3DApp::
getAnimNames(double &tmin, double &tmax) const
{
  std::set<QString> animNameSet;

  auto rootObjects = this->getRootObjects();

  for (auto *rootObject : rootObjects) {
    std::vector<std::string> animNames1;
    rootObject->getAnimationNames(animNames1);

    for (const auto &animName1 : animNames1)
      animNameSet.insert(QString::fromStdString(animName1));

    if (! animNames1.empty())
      rootObject->getAnimationTranslationRange(animNames1[0], tmin, tmax);
  }

  std::vector<QString> animNames;

  for (const auto &animName : animNameSet)
    animNames.push_back(animName);

  return animNames;
}

//---

std::vector<CGeomObject3D *>
CQCamera3DApp::
getRootObjects() const
{
  std::set<CGeomObject3D *>    rootObjectSet;
  std::vector<CGeomObject3D *> rootObjects;

  auto *scene = this->getScene();

  const auto &objects = scene->getObjects();

  for (auto *object : objects) {
    auto *rootObject = object->getRootObject();

    if (rootObjectSet.find(rootObject) == rootObjectSet.end()) {
      rootObjectSet.insert(rootObject);

      rootObjects.push_back(rootObject);
    }
  }

  return rootObjects;
}
