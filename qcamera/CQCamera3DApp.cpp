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

#include <CQMetaEdit.h>
#include <CQAppOptions.h>

#ifdef CQ_PERF_GRAPH
#include <CQPerfGraph.h>
#endif

#include <CGeometry3D.h>
#include <CImportScene.h>
#include <CGeomScene3D.h>
#include <CGeomNodeData.h>
#include <CFile.h>

#include <CQTabSplit.h>
#include <CQUtil.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QTimer>
#include <QKeyEvent>

#include <svg/camera_svg.h>
#include <svg/light_svg.h>

#include <svg/play_svg.h>
#include <svg/pause_svg.h>
#include <svg/play_one_svg.h>

#include <svg/point_select_svg.h>
#include <svg/edge_select_svg.h>
#include <svg/face_select_svg.h>
#include <svg/object_select_svg.h>

#include <svg/depth3d_svg.h>
#include <svg/cull3d_svg.h>
#include <svg/front3d_svg.h>

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

#include <svg/selected_svg.h>
#include <svg/deselected_svg.h>

#include <svg/visible_svg.h>
#include <svg/invisible_svg.h>

#include <svg/menu_svg.h>

#include <set>
#include <iostream>

#define Q(x) #x
#define QUOTE(x) Q(x)

class CQCamera3DEventFilter : public QObject {
 public:
  CQCamera3DEventFilter(CQCamera3DApp *app) :
   app_(app) {
  }

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto *keyEvent = static_cast<QKeyEvent *>(event);

      if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_Home) {
        app_->showMetaEdit();
        return true;
      }

#if 0
      if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_End) {
        CQApp::showPerfDialog();
        return true;
      }

      if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_PageDown) {
        CQApp::showOptions();
        return true;
      }
#endif
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
  }

 private:
  CQCamera3DApp* app_ { nullptr };
};

//---

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

  auto *eventFilter = new CQCamera3DEventFilter(this);

  qApp->installEventFilter(eventFilter);

  //---

  connectSlots(true);

  tabSlot(0);
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
  viewType_ = indToViewType(i);

  Q_EMIT viewTypeChanged();
}

void
CQCamera3DApp::
setCurrentView(const CQCamera3DViewType &viewType)
{
  connectSlots(false);

  viewType_ = viewType;

  tab_->setCurrentIndex(viewTypeToInd(viewType_));

  Q_EMIT viewTypeChanged();

  connectSlots(true);
}

int
CQCamera3DApp::
viewTypeToInd(const CQCamera3DViewType &viewType) const
{
  if      (viewType == CQCamera3DViewType::MODEL    ) return 0;
  else if (viewType == CQCamera3DViewType::OVERVIEW ) return 1;
  else if (viewType == CQCamera3DViewType::UV       ) return 2;
  else if (viewType == CQCamera3DViewType::TEXTURES ) return 3;
  else if (viewType == CQCamera3DViewType::MATERIALS) return 4;
  else if (viewType == CQCamera3DViewType::BONES    ) return 5;
  else if (viewType == CQCamera3DViewType::ANIMATION) return 6;
  else                                                return -1;
}

CQCamera3DViewType
CQCamera3DApp::
indToViewType(int ind) const
{
  if      (ind == 0) return CQCamera3DViewType::MODEL;
  else if (ind == 1) return CQCamera3DViewType::OVERVIEW;
  else if (ind == 2) return CQCamera3DViewType::UV;
  else if (ind == 3) return CQCamera3DViewType::TEXTURES;
  else if (ind == 4) return CQCamera3DViewType::MATERIALS;
  else if (ind == 5) return CQCamera3DViewType::BONES;
  else if (ind == 6) return CQCamera3DViewType::ANIMATION;
  else               return CQCamera3DViewType::NONE;
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

#if 0
void
CQCamera3DApp::
setAnimName(const QString &s)
{
  if (s != animName_) {
    animName_ = s;

    signalAnimTimeChange();
  }
}

void
CQCamera3DApp::
setAnimTime(double r)
{
  if (r != animTime_) {
    animTime_ = r;

    signalAnimTimeChange();
  }
}
#endif

void
CQCamera3DApp::
signalAnimTimeChange()
{
  invalidateNodeMatrices();

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
    std::cerr << "File format not recognised for '" << fileName.toStdString() << "'\n";
    return false;
  }

  im->setInvertX(loadData.invertX);
  im->setInvertY(loadData.invertY);
  im->setInvertZ(loadData.invertZ);

  im->setSwapXY(loadData.swapXY);
  im->setSwapYZ(loadData.swapYZ);
  im->setSwapZX(loadData.swapZX);

  if (loadData.triangulate)
    im->setTriangulate(true);

  if (loadData.textureDir != "")
    im->setTextureDir(loadData.textureDir);

  CFile file(fileName.toStdString());

  if (! im->read(file)) {
    delete im;
    std::cerr << "Failed to read model for '" << fileName.toStdString() << "'\n";
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

QStringList
CQCamera3DApp::
getAnimNames() const
{
  using Objects     = std::vector<CGeomObject3D *>;
  using AnimObjects = std::map<QString, Objects>;

  AnimObjects animObjects;

  auto animObjects1 = this->getAnimObjects();

  for (auto *animObject : animObjects1) {
    std::vector<std::string> animNames;
    animObject->getAnimationNames(animNames);

    for (const auto &animName : animNames) {
      auto animName1 = QString::fromStdString(animName);

      animObjects[animName1].push_back(animObject);
    }
  }

  QStringList animNames;

  for (const auto &pa : animObjects) {
    auto name= pa.first;

    animNames.push_back(name);
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

void
CQCamera3DApp::
stepAnimObjects()
{
  for (auto *animObject : getAnimObjects())
    animObject->stepAnimTime();

  signalAnimTimeChange();
}

std::vector<CGeomObject3D *>
CQCamera3DApp::
getAnimObjects() const
{
  std::set<CGeomObject3D *>    animObjectSet;
  std::vector<CGeomObject3D *> animObjects;

  auto *scene = this->getScene();

  const auto &objects = scene->getObjects();

  for (auto *object : objects) {
    auto *animObject = object->getAnimObject();
    if (! animObject) continue;

    if (animObjectSet.find(animObject) == animObjectSet.end()) {
      animObjectSet.insert(animObject);

      animObjects.push_back(animObject);
    }
  }

  return animObjects;
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

  auto animObjects = getAnimObjects();

  for (auto *animObject : animObjects) {
    if (! animObject->getVisible())
      continue;

    auto animName = animObject->animName();
    if (animName == "") continue;

    auto animTime = animObject->animTime();

    auto &nodeMatrices = objectNodeMatrices[animObject->getInd()];

    animObject->updateNodesAnimationData(animName, animTime);

    auto meshMatrix        = animObject->getMeshGlobalTransform();
    auto inverseMeshMatrix = meshMatrix.inverse();

    //---

    for (const auto &pn : animObject->getNodes()) {
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

//---

void
CQCamera3DApp::
showMetaEdit()
{
  static CQMetaEdit *metaEdit;

  if (! metaEdit)
    metaEdit = new CQMetaEdit;

  metaEdit->show();

  metaEdit->raise();
}

void
CQCamera3DApp::
showPerfDialog()
{
#ifdef CQ_PERF_GRAPH
  auto *dialog = CQPerfDialog::instance();

  dialog->show();
#endif
}

void
CQCamera3DApp::
showAppOptions()
{
  CQAppOptions::show();
}
