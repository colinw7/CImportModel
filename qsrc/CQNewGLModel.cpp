#include <CQNewGLModel.h>
#include <CQNewGLToolbar.h>
#include <CQNewGLStatusBar.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLUVMap.h>
#include <CQNewGLBones.h>
#include <CQNewGLFractalWidget.h>
#include <CQNewGLFont.h>

#include <CGeometry3D.h>
#include <CImportBase.h>

#include <QVBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QTimer>

#define Q(x) #x
#define QUOTE(x) Q(x)

CQNewGLModel::
CQNewGLModel() :
 QFrame()
{
  buildDir_ = QUOTE(BUILD_DIR);

  scene_ = CGeometryInst->createScene3D();

  //---

  timer_ = new QTimer;

  connect(timer_, &QTimer::timeout, this, &CQNewGLModel::timerSlot);

  //---

  addWidgets();
}

//---

void
CQNewGLModel::
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
CQNewGLModel::
startTimer()
{
  timer_->start(100);
}

void
CQNewGLModel::
stopTimer()
{
  timer_->stop();
}

void
CQNewGLModel::
timerSlot()
{
  ++ticks_;

  Q_EMIT timerStep();
}

//---

void
CQNewGLModel::
addWidgets()
{
  auto *layout = new QVBoxLayout(this);

  //---

  toolbar_ = new CQNewGLToolbar(this);

  layout->addWidget(toolbar_);

  //---

  auto *layout1 = new QHBoxLayout;

  layout->addLayout(layout1);

  auto *splitter = new QSplitter;

  layout1->addWidget(splitter);

  //---

  canvas_ = new CQNewGLCanvas(this);
  uvMap_  = new CQNewGLUVMap (this);
  bones_  = new CQNewGLBones (this);

  fractalWidget_ = new CQNewGLFractalWidget(this);

  tab_ = new QTabWidget(this);

  tab_->addTab(canvas_       , "Model"  );
  tab_->addTab(uvMap_        , "UV"     );
  tab_->addTab(bones_        , "Bones"  );
  tab_->addTab(fractalWidget_, "Fractal");

  splitter->addWidget(tab_);

  controlStack_ = new QStackedWidget;

  control_        = new CQNewGLControl(this);
  uvControl_      = new CQNewGLUVControl(this);
  fractalControl_ = new CQNewGLFractalControl(this);

  controlStack_->addWidget(control_);
  controlStack_->addWidget(uvControl_);
  controlStack_->addWidget(fractalControl_);

  splitter->addWidget(controlStack_);

  controlStack_->setVisible(false);

  //---

  statusBar_ = new CQNewGLStatusBar(this);

  layout->addWidget(statusBar_);

  connect(tab_, SIGNAL(currentChanged(int)), this, SLOT(tabSlot(int)));
}

void
CQNewGLModel::
tabSlot(int i)
{
  if      (tab_->tabText(i) == "UV")
    controlStack_->setCurrentIndex(1);
  else if (tab_->tabText(i) == "Fractal")
    controlStack_->setCurrentIndex(2);
  else
    controlStack_->setCurrentIndex(0);
}

void
CQNewGLModel::
setShininess(double r)
{
  shininess_ = r;
}

void
CQNewGLModel::
setShowBone(bool b)
{
  showBone_ = b;
}

void
CQNewGLModel::
update()
{
  control_  ->updateWidgets();
  uvControl_->updateWidgets();
}

void
CQNewGLModel::
updateCamera()
{
  control_->updateCameras();
}

void
CQNewGLModel::
updateLights()
{
  control_->updateLights();
}

//---

bool
CQNewGLModel::
loadModel(const QString &fileName, CGeom3DType format, LoadData &loadData)
{
  static uint modeInd;

  auto modelName = QString("Model.%1").arg(++modeInd);

  // import model
  auto *im = CImportBase::createModel(format, modelName.toStdString());

  if (! im) {
    std::cerr << "Invalid format\n";
    return false;
  }

  im->setInvertX(loadData.invertX);
  im->setInvertY(loadData.invertY);
  im->setInvertZ(loadData.invertZ);

  im->setSwapYZ(loadData.swapYZ);

  if (loadData.fileNameMap != "") {
    if (! im->readFileMap(loadData.fileNameMap))
      std::cerr << "Failed to read file name map\n";
  }

  CFile file(fileName.toStdString());

  if (! im->read(file)) {
    delete im;
    std::cerr << "Failed to read model '" << fileName.toStdString() << "'\n";
    return false;
  }

  auto *scene = im->releaseScene();

  delete im;

  for (auto *object : scene->getObjects()) {
    scene_->addObject(object);

    loadData.objects.push_back(object);
  }

  Q_EMIT modelAdded();

  return true;
}

//---

CBBox3D
CQNewGLModel::
getObjectBBox(CGeomObject3D *object) const
{
  auto bbox = getModelBBox(object);

  auto modelMatrix = object->getHierTransform();

  return transformBBox(bbox, modelMatrix);
}

CBBox3D
CQNewGLModel::
getModelBBox(CGeomObject3D *object) const
{
  CBBox3D bbox;
  object->getModelBBox(bbox, /*hier*/false);

#if 1
  auto modelMatrix = getMeshGlobalTransform(object, /*invert*/false);
//const auto &modelMatrix = object->getHierTransform();

  bbox = transformBBox(bbox, modelMatrix);
#endif

  return bbox;
}

CBBox3D
CQNewGLModel::
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

CMatrix3D
CQNewGLModel::
getMeshGlobalTransform(CGeomObject3D *object, bool invert) const
{
  auto meshMatrix = object->getMeshGlobalTransform();

  return (invert ? meshMatrix.inverse() : meshMatrix);
}

#if 0
CMatrix3D
CQNewGLModel::
getMeshLocalTransform(CGeomObject3D *object, bool invert) const
{
  auto meshMatrix = object->getMeshLocalTransform();

  return (invert ? meshMatrix.inverse() : meshMatrix);
}
#endif
