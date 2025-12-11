#include <CQCamera3DApp.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DOverview.h>
#include <CQCamera3DControl.h>
#include <CQCamera3DStatus.h>
#include <CQCamera3DGeomObject.h>

#include <CGeometry3D.h>
#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CFile.h>

#include <CQTabSplit.h>

#include <QHBoxLayout>

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
};

//---

CQCamera3DApp::
CQCamera3DApp()
{
  buildDir_ = QUOTE(BUILD_DIR);

  //---

  CGeometryInst->setFactory(new CQCamera3DGeomFactory);

  scene_ = CGeometryInst->createScene3D();

  //---

  auto *layout = new QVBoxLayout(this);

  auto *frame = new CQTabSplit;

  layout->addWidget(frame);

  auto *tab = new CQTabSplit;

  tab->setState(CQTabSplit::State::TAB);

  canvas_   = new CQCamera3DCanvas(this);
  overview_ = new CQCamera3DOverview(this);

  tab->addWidget(canvas_  , "3D"      );
  tab->addWidget(overview_, "Overview");

  control_  = new CQCamera3DControl(this);

  frame->addWidget(tab     , "Model"  );
  frame->addWidget(control_, "Control");

  status_ = new CQCamera3DStatus(this);

  layout->addWidget(status_);

  //---

  connect(canvas_, SIGNAL(textureAdded()), this, SIGNAL(textureAdded()));
}

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

  for (auto *object : scene->getObjects()) {
    scene_->addObject(object);

    loadData.objects.push_back(object);
  }

  Q_EMIT modelAdded();

  return true;
}
