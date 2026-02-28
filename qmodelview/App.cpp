#include <App.h>
#include <Canvas.h>
#include <Control.h>
#include <Overview.h>
#include <GeomObject.h>
#include <Texture.h>

#include <CQTabSplit.h>
#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGeometry3D.h>

#include <QVBoxLayout>

#define Q(x) #x
#define QUOTE(x) Q(x)

namespace CQModel3DView {

class GeomFactory : public CGeometryFactory {
 public:
  GeomFactory() { }
 ~GeomFactory() override { }

  CGeomObject3D *createObject3D(CGeomScene3D *pscene, const std::string &name) const override {
    return new GeomObject(pscene, name);
  }

#if 0
  CGeomFace3D *createFace3D() const override {
    return new GeomFace;
  }

  CGeomLine3D *createLine3D() const override {
    return new GeomLine;
  }

  CGeomLight3D *createLight3D(CGeomScene3D *pscene, const std::string &name) const override {
    return new Light(pscene, name);
  }
#endif

  CGeomTexture *createTexture() const override {
    return new Texture;
  }
};

}

//---

namespace CQModel3DView {

App::
App()
{
  setObjectName("app");

  buildDir_ = QUOTE(BUILD_DIR);

  //---

  CGeometry3DInst->setFactory(new GeomFactory);

  scene_ = CGeometry3DInst->createScene3D();

  //---

  auto *layout = new QHBoxLayout(this);

  auto *tab = new CQTabSplit;

  tab->setState(CQTabSplit::State::TAB);

  canvas_   = new Canvas  (this);
  overview_ = new Overview(this);

  tab->addWidget(canvas_  , "3D View" );
  tab->addWidget(overview_, "Overview");

  layout->addWidget(tab);

  control_ = new Control(this);

  layout->addWidget(control_);
}

bool
App::
loadModel(const QString &fileName, CGeom3DType format)
{
  static uint modeInd;

  auto modelName = QString("Model.%1").arg(++modeInd);

  auto *im = CImportBase::createModel(format, modelName.toStdString());

  if (! im) {
    std::cerr << "File format not recognised for '" << fileName.toStdString() << "'\n";
    return false;
  }

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

  std::vector<CGeomObject3D *> objects;

  if (numTop > 1) {
    auto *parentObj = CGeometry3DInst->createObject3D(scene_, modelName.toStdString());

    scene_->addObject(parentObj);

    //objects.push_back(parentObj);

    for (auto *object : scene->getObjects()) {
      scene_->addObject(object);

      if (! object->parent())
        parentObj->addChild(object);

      objects.push_back(object);
    }
  }
  else {
    for (auto *object : scene->getObjects()) {
      scene_->addObject(object);

      objects.push_back(object);
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

}
