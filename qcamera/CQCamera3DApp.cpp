#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DUVMap.h>
#include <CQCamera3DTextures.h>
#include <CQCamera3DMaterials.h>
#include <CQCamera3DBones.h>
#include <CQCamera3DControl.h>
#include <CQCamera3DToolbar.h>
#include <CQCamera3DSidebar.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DTexture.h>

#include <CQCamera3DGeomObject.h>
#include <CQCamera3DGeomFace.h>
#include <CQCamera3DGeomLine.h>

#include <CGeometry3D.h>
#include <CImportScene.h>
#include <CGeomScene3D.h>
#include <CGeomNodeData.h>
#include <CFile.h>

#include <CQTabSplit.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QTimer>

#include <svg/light_svg.h>
#include <svg/play_svg.h>
#include <svg/pause_svg.h>
#include <svg/play_one_svg.h>

#include <svg/point_select_svg.h>
#include <svg/edge_select_svg.h>
#include <svg/face_select_svg.h>

#include <svg/wireframe_svg.h>
#include <svg/solid_fill_svg.h>
#include <svg/texture_fill_svg.h>

#include <svg/select_svg.h>
#include <svg/cursor_svg.h>
#include <svg/move_svg.h>
#include <svg/rotate_svg.h>
#include <svg/scale_svg.h>

#include <svg/extrude_svg.h>
#include <svg/loop_cut_svg.h>

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

  CGeomFace3D *createFace3D() const override {
    return new CQCamera3DGeomFace;
  }

  CGeomLine3D *createLine3D() const override {
    return new CQCamera3DGeomLine;
  }

  CGeomLight3D *createLight3D(CGeomScene3D *pscene, const std::string &name) const override {
    return new CQCamera3DLight(pscene, name);
  }

  CGeomTexture *createTexture() const override {
    return new CQCamera3DTexture;
  }
};

//---

CQCamera3DApp::
CQCamera3DApp()
{
  setObjectName("app");

  buildDir_ = QUOTE(BUILD_DIR);

  //---

  CGeometry3DInst->setFactory(new CQCamera3DGeomFactory);

  scene_ = CGeometry3DInst->createScene3D();

  //---

  canvas_    = new CQCamera3DCanvas(this);
  overview_  = new CQCamera3DOverview(this);
  uvMap_     = new CQCamera3DUVMap(this);
  textures_  = new CQCamera3DTextures(this);
  materials_ = new CQCamera3DMaterials(this);
  bones_     = new CQCamera3DBones(this);
  animation_ = new QFrame(this);

  //---

  // vertical layout: toolbar, central, status
  auto *layout = new QVBoxLayout(this);

  toolbar_ = new CQCamera3DToolbar(this);

  layout->addWidget(toolbar_);

  // center frame : sidebar, views, control
  auto *centralFrame = CQUtil::makeWidget<QFrame>("centralFrame");
  auto *centralLayout = new QHBoxLayout(centralFrame);

  layout->addWidget(centralFrame);

  sidebar_ = new CQCamera3DSidebar(this);

  centralLayout->addWidget(sidebar_);

  auto *frame = CQUtil::makeWidget<CQTabSplit>("topTab");

  centralLayout->addWidget(frame);

  tab_ = CQUtil::makeWidget<CQTabSplit>("centerTab");

  tab_->setState(CQTabSplit::State::TAB);

  tab_->addWidget(canvas_   , "3D"       );
  tab_->addWidget(overview_ , "Overview" );
  tab_->addWidget(uvMap_    , "UV"       );
  tab_->addWidget(textures_ , "Textures" );
  tab_->addWidget(materials_, "Materials");
  tab_->addWidget(bones_    , "Bones"    );
  tab_->addWidget(animation_, "Animation");

  control_ = new CQCamera3DControl(this);

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
  CQUtil::connectDisconnect(b, canvas_, SIGNAL(materialAdded()),
                            this, SIGNAL(materialAdded()));
}

void
CQCamera3DApp::
tabSlot(int i)
{
  control_->setCurrentControl(i);

  if      (i == 0)
    sidebar_->setView(CQCamera3DSidebar::View::CANVAS);
  else if (i == 1)
    sidebar_->setView(CQCamera3DSidebar::View::OVERVIEW);
  else
    sidebar_->setView(CQCamera3DSidebar::View::NONE);
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
setCurrentTextureId(int id)
{
  currentTextureId_ = id;

  Q_EMIT currentTextureChanged();
}

//---

void
CQCamera3DApp::
setCurrentMaterial(const QString &name)
{
  currentMaterial_ = name;

  Q_EMIT currentMaterialChanged();
}

CGeomMaterial *
CQCamera3DApp::
getMaterialByName(const std::string &name) const
{
  auto *scene = getScene();

  for (auto *material : scene->getMaterials()) {
    if (name == material->name())
      return material;
  }

  return nullptr;
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
  if (s != animName_) {
    animName_ = s;

    objectNodeMatricesValid_ = false;

    Q_EMIT animNameChanged();
  }
}

void
CQCamera3DApp::
setAnimTime(double r)
{
  if (r != animTime_) {
    animTime_ = r;

    objectNodeMatricesValid_ = false;

    Q_EMIT animTimeChanged();
  }
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
    auto *parentObj = CGeometry3DInst->createObject3D(scene_, modelName.toStdString());

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

  for (auto *texture : scene->textures()) {
    scene_->addTexture(texture);
  }

  Q_EMIT modelAdded();

  return true;
}

//---

CGeomTexture *
CQCamera3DApp::
getTextureByName(const std::string &name) const
{
#if 0
  auto name1   = name;
  bool flipped = false;

  auto len = name1.size();

  if (len > 5 && name1.substr(len - 5) == ".flip") {
    name1   = name1.substr(0, len - 5);
    flipped = true;
  }

  auto *scene = this->getScene();

  auto *texture = scene->getTextureByName(name1);
  if (! texture) return nullptr;

  auto *texture1 = dynamic_cast<CQCamera3DTexture *>(texture);
  assert(texture1);

  texture1->setFlipped(flipped);

  return texture;
#else
  auto *scene = this->getScene();

  auto *texture = scene->getTextureByName(name);
  if (! texture) return nullptr;

  return texture;
#endif
}

CGeomTexture *
CQCamera3DApp::
getTextureById(int id) const
{
  auto *scene = this->getScene();

  auto *texture = scene->getTextureById(id);
  if (! texture) return nullptr;

  return texture;
}

//---

std::vector<CQCamera3DApp::AnimData>
CQCamera3DApp::
getAnimNames() const
{
  using Objects     = std::vector<CGeomObject3D *>;
  using AnimObjects = std::map<QString, Objects>;

  AnimObjects animObjects;

  auto rootObjects = this->getRootObjects();

  for (auto *rootObject : rootObjects) {
    std::vector<std::string> animNames1;
    rootObject->getAnimationNames(animNames1);

    for (const auto &animName1 : animNames1) {
      auto animName2 = QString::fromStdString(animName1);

      animObjects[animName2].push_back(rootObject);
    }
  }

  std::vector<AnimData> animNames;

  for (const auto &pa : animObjects) {
    AnimData animData;

    animData.name = pa.first;

    auto *rootObject = pa.second[0];

    rootObject->getAnimationTranslationRange(animData.name.toStdString(),
                                             animData.tmin, animData.tmax);

    animNames.push_back(animData);
  }

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

//---

const CQCamera3DApp::NodeMatrices &
CQCamera3DApp::
getObjectNodeMatrices(CGeomObject3D *object) const
{
  const auto &objectNodeMatrices = getNodeMatrices();

  auto po = objectNodeMatrices.find(object->getInd());
  assert(po != objectNodeMatrices.end());

  return (*po).second;
}

const CQCamera3DApp::ObjectNodeMatrices &
CQCamera3DApp::
getNodeMatrices() const
{
  auto *th = const_cast<CQCamera3DApp *>(this);

  if (! th->objectNodeMatricesValid_) {
    th->objectNodeMatrices_ = calcNodeMatrices();

    th->objectNodeMatricesValid_ = true;
  }

  return objectNodeMatrices_;
}

CQCamera3DApp::ObjectNodeMatrices
CQCamera3DApp::
calcNodeMatrices() const
{
  ObjectNodeMatrices objectNodeMatrices;

  auto animName = this->animName().toStdString();
  auto animTime = this->animTime();

  auto rootObjects = getRootObjects();

  for (auto *rootObject : rootObjects) {
    if (! rootObject->getVisible())
      continue;

    auto animName1 = rootObject->animName();

    if (animName1 == "")
      animName1 = animName;

    auto &nodeMatrices = objectNodeMatrices[rootObject->getInd()];

    rootObject->updateNodesAnimationData(animName1, animTime);

    auto meshMatrix        = rootObject->getMeshGlobalTransform();
    auto inverseMeshMatrix = meshMatrix.inverse();

    //---

    for (const auto &pn : rootObject->getNodes()) {
      auto &node = const_cast<CGeomNodeData &>(pn.second);
      //if (! node.isJoint()) continue;

      nodeMatrices[node.index()] = node.calcNodeAnimMatrix(inverseMeshMatrix);
    }
  }

  return objectNodeMatrices;
}

CPoint3D
CQCamera3DApp::
adjustAnimPoint(const CGeomVertex3D &vertex, const CPoint3D &p,
                const NodeMatrices &nodeMatrices) const
{
  const auto &jointData = vertex.getJointData();

  struct NodeWeight {
    int    nodeId { -1 };
    double weight { 0.0 };
  };

  std::vector<NodeWeight> nodeWeights;

  double total = 0.0;

  for (int i = 0; i < 4; ++i) {
    if (jointData.nodeDatas[i].node >= 0 && jointData.nodeDatas[i].weight > 0.0) {
      NodeWeight nodeWeight;

      nodeWeight.nodeId = jointData.nodeDatas[i].node;
      nodeWeight.weight = jointData.nodeDatas[i].weight;

      nodeWeights.push_back(nodeWeight);

      total += nodeWeight.weight;
    }
  }

  auto f = (total > 0.0 ? 1.0/total : 1.0);

  if (! nodeWeights.empty()) {
    for (auto &nodeWeight : nodeWeights)
      nodeWeight.weight *= f;

    auto p1 = CPoint3D(0, 0, 0);

    for (const auto &nodeWeight : nodeWeights) {
      auto pm = nodeMatrices.find(nodeWeight.nodeId);

      if (pm != nodeMatrices.end()) {
        const auto &boneTransform = (*pm).second;

        p1 += nodeWeight.weight*(boneTransform*p);
      }
      else
        p1 += nodeWeight.weight*p;
    }

    return p1;
  }
  else
    return p;
}

//---

bool
CQCamera3DApp::
writeScene(CGeomScene3D *scene) const
{
  CFile file("temp.scene");

  CImportScene im;

  return im.write(&file, scene);
}

//---

CBBox3D
CQCamera3DApp::
transformBBox(const CBBox3D &bbox, const CMatrix3D &matrix) const
{
  const auto &pmin = bbox.getMin();
  const auto &pmax = bbox.getMax();

  std::vector<CPoint3D> points;

  auto addPoint = [&](double x, double y, double z) {
    points.push_back(CPoint3D(x, y, z));
  };

  addPoint(pmin.getX(), pmin.getY(), pmin.getZ());
  addPoint(pmax.getX(), pmin.getY(), pmin.getZ());
  addPoint(pmax.getX(), pmax.getY(), pmin.getZ());
  addPoint(pmin.getX(), pmax.getY(), pmin.getZ());
  addPoint(pmin.getX(), pmin.getY(), pmax.getZ());
  addPoint(pmax.getX(), pmin.getY(), pmax.getZ());
  addPoint(pmax.getX(), pmax.getY(), pmax.getZ());
  addPoint(pmin.getX(), pmax.getY(), pmax.getZ());

  CBBox3D bbox1;

  for (const auto &p : points)
    bbox1.add(matrix*p);

  return bbox1;
}
