#include <CQNewGLControl.h>
#include <CQNewGLModel.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLUVMap.h>
#include <CQNewGLCamera.h>
#include <CQNewGLLight.h>
#include <CQNewGLAxes.h>
#include <CQNewGLBBox.h>
#include <CQNewGLNormals.h>
#include <CQNewGLHull.h>
#include <CQNewGLTerrain.h>
#include <CQNewGLMaze.h>
#include <CQNewGLSkybox.h>
#include <CQNewGLEmitter.h>
#include <CQNewGLFractal.h>
#include <CQNewGLDrawTree.h>
#include <CQNewGLShape.h>
#include <CQNewGLUtil.h>

#include <CImportBase.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>
#include <CQGLTexture.h>
#include <CQUtil.h>

#include <CQColorEdit.h>
#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQSlider.h>
#include <CQPoint3DEdit.h>
#include <CQMatrix3D.h>
#include <CQImage.h>
#include <CQIconButton.h>

#include <QTabWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>

#include <svg/play_svg.h>
#include <svg/pause_svg.h>
#include <svg/play_one_svg.h>

CQNewGLControl::
CQNewGLControl(CQNewGLModel *app) :
 app_(app)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  //---

  auto *layout = new QVBoxLayout(this);

  //---

  tab_ = new QTabWidget;

  tab_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  layout->addWidget(tab_);

  //---

  modelTab_ = new QTabWidget;

  modelTab_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  tab_->addTab(modelTab_, "Scene");

  //---

  int modelTabNum = 0;

  auto addModelTab = [&](QWidget *w, const QString &label) {
    modelTabName_[modelTabNum++] = label;

    modelTab_->addTab(w, label);
  };

  generalControl_ = new CQNewGLGeneralControl(this);

  addModelTab(generalControl_, "General");

  objectsControl_ = new CQNewGLObjectsControl(this);

  addModelTab(objectsControl_, "Models");

  cameraControl_ = new CQNewGLCameraControl(this);

  addModelTab(cameraControl_, "Cameras");

  lightsControl_ = new CQNewGLLightsControl(this);

  addModelTab(lightsControl_, "Lights");

  normalsControl_ = new CQNewGLNormalsControl(this);

  addModelTab(normalsControl_, "Normals");

  axesControl_ = new CQNewGLAxesControl(this);

  addModelTab(axesControl_, "Axes");

  bboxControl_ = new CQNewGLBBoxControl(this);

  addModelTab(bboxControl_, "BBox");

  hullControl_ = new CQNewGLHullControl(this);

  addModelTab(hullControl_, "Hull");

  basisControl_ = new CQNewGLBasisControl(this);

  addModelTab(basisControl_, "Basis");

  texturesControl_ = new CQNewGLTexturesControl(this);

  addModelTab(texturesControl_, "Textures");

  uvControl_ = new CQNewGLUVControl(this);

  addModelTab(uvControl_, "UV");

  animControl_ = new CQNewGLAnimControl(this);

  addModelTab(animControl_, "Animation");

  bonesControl_ = new CQNewGLBonesControl(this);

  addModelTab(bonesControl_, "Bones");

  //---

  shapesTab_ = new QTabWidget;

  shapesTab_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  tab_->addTab(shapesTab_, "Extras");

  int shapesTabNum = 0;

  auto addShapesTab = [&](QWidget *w, const QString &label) {
    shapesTabName_[shapesTabNum++] = label;

    shapesTab_->addTab(w, label);
  };

  //---

  terrainControl_ = new CQNewGLTerrainControl(this);

  addShapesTab(terrainControl_, "Terrain");

  mazeControl_ = new CQNewGLMazeControl(this);

  addShapesTab(mazeControl_, "Maze");

  skyboxControl_ = new CQNewGLSkyboxControl(this);

  addShapesTab(skyboxControl_, "Skybox");

  emitterControl_ = new CQNewGLEmitterControl(this);

  addShapesTab(emitterControl_, "Emitter");

  fractalControl_ = new CQNewGLFractalControl(this);

  addShapesTab(fractalControl_, "Fractal");

  drawTreeControl_ = new CQNewGLDrawTreeControl(this);

  addShapesTab(drawTreeControl_, "Draw Tree");

  shapeControl_ = new CQNewGLShapeControl(this);

  addShapesTab(shapeControl_, "Shape");

  //---

  layout->addStretch(1);

  //---

  connect(modelTab_, SIGNAL(currentChanged(int)), this, SLOT(modelTabSlot(int)));

  connect(canvas(), SIGNAL(textureAdded()), this, SLOT(invalidateTextures()));
  connect(canvas(), SIGNAL(cameraAdded()), this, SLOT(invalidateCameras()));
  connect(canvas(), SIGNAL(lightAdded()), this, SLOT(invalidateLights()));
//connect(canvas(), SIGNAL(shapeAdded()), this, SLOT(invalidateShapes()));
}

CQNewGLCanvas *
CQNewGLControl::
canvas() const
{
  return app()->canvas();
}

CQNewGLUVMap *
CQNewGLControl::
uvMap() const
{
  return app()->uvMap();
}

void
CQNewGLControl::
modelTabSlot(int ind)
{
  auto tabName = modelTabName_[ind];

  bool isShowBone = app_->isShowBone();

  app_->setShowUVMap(tabName == "UV");
  app_->setShowBone (tabName == "Bones");

  if (app_->isShowBone() != isShowBone) {
    canvas()->updateObjectsData();
    canvas()->update();
  }
}

void
CQNewGLControl::
updateWidgets()
{
  updateGeneral();

  updateObjects();

  updateCameras();
  updateLights();
  updateNormals();

  updateAxes();
  updateBBox();
  updateHull();

  updateTextures();
  updateUV();
  updateAnim();
  updateBones();

  updateTerrain();
  updateMaze();

  updateSkybox();

  updateEmitter();

  updateFractal();

  updateDrawTree();

  updateShape();
}

void
CQNewGLControl::
invalidateCameras()
{
  cameraControl_->invalidateCameras();
}

void
CQNewGLControl::
invalidateLights()
{
  lightsControl_->invalidateLights();
}

void
CQNewGLControl::
invalidateTextures()
{
  texturesControl_->invalidateTextures();
}

void
CQNewGLControl::
invalidateShapes()
{
  shapeControl_->invalidateShapes();
}

void
CQNewGLControl::
updateGeneral()
{
  generalControl_->updateWidgets();
}

void
CQNewGLControl::
updateObjects()
{
  objectsControl_->updateWidgets();
  animControl_   ->updateWidgets();
}

void
CQNewGLControl::
updateCameras()
{
  cameraControl_->updateWidgets();
}

void
CQNewGLControl::
updateLights()
{
  lightsControl_->updateWidgets();
}

void
CQNewGLControl::
updateNormals()
{
  normalsControl_->updateWidgets();
}

void
CQNewGLControl::
updateAxes()
{
  axesControl_->updateWidgets();
}

void
CQNewGLControl::
updateBBox()
{
  bboxControl_->updateWidgets();
}

void
CQNewGLControl::
updateHull()
{
  hullControl_->updateWidgets();
}

void
CQNewGLControl::
updateTextures()
{
  texturesControl_->updateWidgets();
}

void
CQNewGLControl::
updateUV()
{
  uvControl_->updateObjects();
}

void
CQNewGLControl::
updateAnim()
{
//animControl_->updateObjects();
  animControl_->updateWidgets();
}

void
CQNewGLControl::
updateBones()
{
  bonesControl_->updateWidgets();
}

void
CQNewGLControl::
updateTerrain()
{
  terrainControl_->updateWidgets();
}

void
CQNewGLControl::
updateMaze()
{
  mazeControl_->updateWidgets();
}

void
CQNewGLControl::
updateSkybox()
{
  skyboxControl_->updateWidgets();
}

void
CQNewGLControl::
updateEmitter()
{
  emitterControl_->updateWidgets();
}

void
CQNewGLControl::
updateFractal()
{
  fractalControl_->updateWidgets();
}

void
CQNewGLControl::
updateDrawTree()
{
  drawTreeControl_->updateWidgets();
}

void
CQNewGLControl::
updateShape()
{
  shapeControl_->updateWidgets();
}

CGeomObject3D *
CQNewGLControl::
getRootObject() const
{
  auto *object = canvas()->getObject(0);

  while (object && object->parent())
    object = object->parent();

  return object;
}

//---

CQNewGLGeneralControl::
CQNewGLGeneralControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  auto addLabelEdit = [&](const QString &label, QWidget *w) {
    auto *frame   = new QFrame;
    auto *layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  //---

  colorEdit_ = new CQColorEdit(this);

  addLabelEdit("Bg Color", colorEdit_);

  //---

  auto *glOptionsGroup  = new QGroupBox("GL Options");
  auto *glOptionsLayout = new QVBoxLayout(glOptionsGroup);

  layout->addWidget(glOptionsGroup);

  depthTestCheck_ = new QCheckBox("Depth Test");

  glOptionsLayout->addWidget(depthTestCheck_);

  //---

  cullCheck_ = new QCheckBox("Cull Face");

  glOptionsLayout->addWidget(cullCheck_);

  //---

  frontFaceCheck_ = new QCheckBox("Front Face");

  glOptionsLayout->addWidget(frontFaceCheck_);

  //---

  polygonSolidCheck_ = new QCheckBox("Polygon Solid");

  layout->addWidget(polygonSolidCheck_);

  //---

  polygonLineCheck_ = new QCheckBox("Polygon Line");

  layout->addWidget(polygonLineCheck_);

  //---

  flipYZCheck_ = new QCheckBox("Flip YZ");

  layout->addWidget(flipYZCheck_);

  //---

  invertDepthCheck_ = new QCheckBox("Invert Depth");

  layout->addWidget(invertDepthCheck_);

  //---

  auto *controlGroup  = new QGroupBox("Control");
  auto *controlLayout = new QVBoxLayout(controlGroup);

  layout->addWidget(controlGroup);

  //---

  auto *buttonGroup = new QButtonGroup;

  auto *modelCheck  = new QRadioButton("Model");
  auto *lightCheck  = new QRadioButton("Light");
  auto *cameraCheck = new QRadioButton("Camera");

  buttonGroup->addButton(modelCheck);
  buttonGroup->addButton(lightCheck);
  buttonGroup->addButton(cameraCheck);

  buttonGroup->setId(modelCheck , int(CQNewGLModel::Type::MODEL ));
  buttonGroup->setId(lightCheck , int(CQNewGLModel::Type::LIGHT ));
  buttonGroup->setId(cameraCheck, int(CQNewGLModel::Type::CAMERA));

  cameraCheck->setChecked(true);

  connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(typeSlot(int)));

  controlLayout->addWidget(modelCheck);
  controlLayout->addWidget(lightCheck);
  controlLayout->addWidget(cameraCheck);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLGeneralControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, colorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(bgColorSlot(const QColor &)));

  if (b) {
    connect(depthTestCheck_, SIGNAL(stateChanged(int)), this, SLOT(depthTestSlot(int)));

    connect(cullCheck_, SIGNAL(stateChanged(int)), this, SLOT(cullSlot(int)));

    connect(frontFaceCheck_, SIGNAL(stateChanged(int)), this, SLOT(frontFaceSlot(int)));

    connect(polygonSolidCheck_, SIGNAL(stateChanged(int)), this, SLOT(polygonSolidSlot(int)));

    connect(polygonLineCheck_, SIGNAL(stateChanged(int)), this, SLOT(polygonLineSlot(int)));

    connect(flipYZCheck_, SIGNAL(stateChanged(int)), this, SLOT(flipYZSlot(int)));

    connect(invertDepthCheck_, SIGNAL(stateChanged(int)), this, SLOT(invertDepthSlot(int)));
  }
  else {
    disconnect(depthTestCheck_, SIGNAL(stateChanged(int)), this, SLOT(depthTestSlot(int)));

    disconnect(cullCheck_, SIGNAL(stateChanged(int)), this, SLOT(cullSlot(int)));

    disconnect(frontFaceCheck_, SIGNAL(stateChanged(int)), this, SLOT(frontFaceSlot(int)));

    disconnect(polygonSolidCheck_, SIGNAL(stateChanged(int)), this, SLOT(polygonSolidSlot(int)));

    disconnect(polygonLineCheck_, SIGNAL(stateChanged(int)), this, SLOT(polygonLineSlot(int)));

    disconnect(flipYZCheck_, SIGNAL(stateChanged(int)), this, SLOT(flipYZSlot(int)));

    disconnect(invertDepthCheck_, SIGNAL(stateChanged(int)), this, SLOT(invertDepthSlot(int)));
  }
}

void
CQNewGLGeneralControl::
updateWidgets()
{
  auto *canvas = this->canvas();

  connectSlots(false);

  colorEdit_->setColor(canvas->bgColor());

  depthTestCheck_->setChecked(canvas->isDepthTest());
  cullCheck_     ->setChecked(canvas->isCullFace());
  frontFaceCheck_->setChecked(canvas->isFrontFace());

  polygonSolidCheck_->setChecked(canvas->isPolygonSolid());
  polygonLineCheck_ ->setChecked(canvas->isPolygonLine());

  flipYZCheck_     ->setChecked(canvas->isFlipYZ());
  invertDepthCheck_->setChecked(canvas->isInvertDepth());

  connectSlots(true);
}

void
CQNewGLGeneralControl::
bgColorSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setBgColor(c);
  canvas->update();
}

void
CQNewGLGeneralControl::
depthTestSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setDepthTest(state);
  canvas->update();
}

void
CQNewGLGeneralControl::
cullSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setCullFace(state);
  canvas->update();
}

void
CQNewGLGeneralControl::
frontFaceSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setFrontFace(state);
  canvas->update();
}

void
CQNewGLGeneralControl::
polygonSolidSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setPolygonSolid(state);
  canvas->update();
}

void
CQNewGLGeneralControl::
polygonLineSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setPolygonLine(state);
  canvas->update();
}

void
CQNewGLGeneralControl::
flipYZSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setFlipYZ(state);
}

void
CQNewGLGeneralControl::
invertDepthSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setInvertDepth(state);
}

void
CQNewGLGeneralControl::
typeSlot(int i)
{
  auto *app = control_->app();

  app->setType(static_cast<CQNewGLModel::Type>(i));
}

//---

CQNewGLCameraControl::
CQNewGLCameraControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  camerasList_ = new QListWidget;

  camerasList_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(camerasList_);

  //---

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QGridLayout(controlFrame);

  layout->addWidget(controlFrame);

  int row = 0;

  auto addLabelEdit = [&](const QString &label, auto *w) {
    controlLayout->addWidget(new QLabel(label), row, 0);
    controlLayout->addWidget(w, row, 1);
    ++row;
    return w;
  };

  //---

  showCheck_    = addLabelEdit("Show"    , new QCheckBox);
  followCheck_  = addLabelEdit("Follow"  , new QCheckBox);
  eyelineCheck_ = addLabelEdit("Eye Line", new QCheckBox);

  //---

  orthoCheck_ = addLabelEdit("Ortho", new QCheckBox);

  //---

  updateCurrentCheck_ = new QCheckBox;
  addLabelEdit("Update Current", updateCurrentCheck_);

  rotateCheck_ = new QCheckBox;
  addLabelEdit("Rotate", rotateCheck_);

  rotateAtCombo_ = new QComboBox;
  addLabelEdit("Rotate At", rotateAtCombo_);

  strafeCheck_ = new QCheckBox;
  addLabelEdit("Strafe", strafeCheck_);

  rotateAtCombo_->addItems(QStringList() << "Camera" << "Origin");

  //---

  zoomEdit_ = new CQRealSpin;
  addLabelEdit("Zoom", zoomEdit_);

  nearEdit_ = new CQRealSpin;
  addLabelEdit("Near", nearEdit_);

  farEdit_ = new CQRealSpin;
  addLabelEdit("Far", farEdit_);

  yawEdit_ = new CQRealSpin;
  addLabelEdit("Yaw", yawEdit_);

  pitchEdit_ = new CQRealSpin;
  addLabelEdit("Pitch", pitchEdit_);

  rollEdit_ = new CQRealSpin;
  addLabelEdit("Roll", rollEdit_);

  //---

  speedEdit_ = new CQRealSpin;
  addLabelEdit("Speed", speedEdit_);

  //---

  originEdit_ = new CQPoint3DEdit;
  originEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Origin", originEdit_);

  positionEdit_ = new CQPoint3DEdit;
  positionEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Position", positionEdit_);

  upEdit_ = new CQPoint3DEdit;
  upEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Up", upEdit_);

  rightEdit_ = new CQPoint3DEdit;
  rightEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Right", rightEdit_);

  //---

  matrixEdit_ = new CQMatrix3D;
  matrixEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Matrix", matrixEdit_);

  //---

  colorEdit_ = new CQColorEdit;
  addLabelEdit("Color", colorEdit_);

  //---

  controlLayout->setRowStretch(row, 1);

  //--

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *addButton   = new QPushButton("Add");
  auto *resetButton = new QPushButton("Reset");
  auto *initButton  = new QPushButton("Init");

  connect(addButton, &QPushButton::clicked, this, &CQNewGLCameraControl::addSlot);
  connect(resetButton, &QPushButton::clicked, this, &CQNewGLCameraControl::resetSlot);
  connect(initButton, &QPushButton::clicked, this, &CQNewGLCameraControl::initSlot);

  buttonsLayout->addWidget(addButton);
  buttonsLayout->addWidget(resetButton);
  buttonsLayout->addWidget(initButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLCameraControl::
connectSlots(bool b)
{
  if (b) {
    connect(showCheck_, &QCheckBox::stateChanged, this,
            &CQNewGLCameraControl::showSlot);
    connect(followCheck_, &QCheckBox::stateChanged, this,
            &CQNewGLCameraControl::followSlot);
    connect(eyelineCheck_, &QCheckBox::stateChanged, this,
            &CQNewGLCameraControl::eyelineSlot);

    connect(orthoCheck_, SIGNAL(stateChanged(int)), this, SLOT(orthoSlot(int)));

    connect(camerasList_, &QListWidget::currentItemChanged,
            this, &CQNewGLCameraControl::cameraSelectedSlot);

    connect(updateCurrentCheck_, &QCheckBox::stateChanged, this,
            &CQNewGLCameraControl::updateCurrentSlot);

    connect(rotateCheck_, &QCheckBox::stateChanged, this, &CQNewGLCameraControl::rotateSlot);
    connect(rotateAtCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(rotateAtSlot(int)));
    connect(strafeCheck_, &QCheckBox::stateChanged, this, &CQNewGLCameraControl::strafeSlot);

    connect(zoomEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::zoomSlot);
    connect(nearEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::nearSlot);
    connect(farEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::farSlot);
    connect(yawEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::yawSlot);
    connect(pitchEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::pitchSlot);
    connect(rollEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::rollSlot);

    connect(speedEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::speedSlot);

    connect(originEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLCameraControl::originSlot);
    connect(positionEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLCameraControl::positionSlot);
    connect(upEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLCameraControl::upSlot);
    connect(rightEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLCameraControl::rightSlot);

    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(colorSlot(const QColor &)));
  }
  else {
    disconnect(showCheck_, &QCheckBox::stateChanged, this,
               &CQNewGLCameraControl::showSlot);
    disconnect(followCheck_, &QCheckBox::stateChanged, this,
               &CQNewGLCameraControl::followSlot);
    disconnect(eyelineCheck_, &QCheckBox::stateChanged, this,
               &CQNewGLCameraControl::eyelineSlot);

    disconnect(orthoCheck_, SIGNAL(stateChanged(int)), this, SLOT(orthoSlot(int)));

    disconnect(camerasList_, &QListWidget::currentItemChanged,
               this, &CQNewGLCameraControl::cameraSelectedSlot);

    disconnect(updateCurrentCheck_, &QCheckBox::stateChanged, this,
               &CQNewGLCameraControl::updateCurrentSlot);

    disconnect(rotateCheck_, &QCheckBox::stateChanged, this, &CQNewGLCameraControl::rotateSlot);
    disconnect(rotateAtCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(rotateAtSlot(int)));
    disconnect(strafeCheck_, &QCheckBox::stateChanged, this, &CQNewGLCameraControl::strafeSlot);

    disconnect(zoomEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::zoomSlot);
    disconnect(nearEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::nearSlot);
    disconnect(farEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::farSlot);
    disconnect(yawEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::yawSlot);
    disconnect(pitchEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::pitchSlot);
    disconnect(rollEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::rollSlot);

    disconnect(speedEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::speedSlot);

    disconnect(originEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLCameraControl::originSlot);
    disconnect(positionEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLCameraControl::positionSlot);
    disconnect(upEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLCameraControl::upSlot);
    disconnect(rightEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLCameraControl::rightSlot);

    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(colorSlot(const QColor &)));
  }
}

CQNewGLCamera *
CQNewGLCameraControl::
camera() const
{
  if (updateCurrent_)
    return canvas()->getCurrentCamera();
  else
    return canvas()->getCamera(currentCamera_);
}

void
CQNewGLCameraControl::
showSlot(int b)
{
  auto *canvas = this->canvas();

  canvas->setShowCameras(b);
  canvas->update();
}

void
CQNewGLCameraControl::
followSlot(int b)
{
  auto *canvas = this->canvas();

  camera()->setFollowObject(b);
  canvas->update();
}

void
CQNewGLCameraControl::
eyelineSlot(int b)
{
  auto *canvas = this->canvas();

  canvas->setShowEyeline(b);
}

void
CQNewGLCameraControl::
orthoSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setOrtho(state);
}

void
CQNewGLCameraControl::
cameraSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = this->canvas();

  currentCamera_ = item->data(Qt::UserRole).toInt();

  if (updateCurrent_)
    canvas->setCurrentCamera(currentCamera_);

  updateWidgets();

  canvas->update();
}

void
CQNewGLCameraControl::
updateCurrentSlot(int b)
{
  updateCurrent_ = b;
}

void
CQNewGLCameraControl::
zoomSlot(double r)
{
  camera()->setZoom(r);
  canvas()->update();
}

void
CQNewGLCameraControl::
nearSlot(double r)
{
  camera()->setNear(r);
  canvas()->update();
}

void
CQNewGLCameraControl::
rotateSlot(int b)
{
  canvas()->setCameraRotate(b);
  canvas()->update();
}

void
CQNewGLCameraControl::
rotateAtSlot(int i)
{
  canvas()->setCameraRotateAt(i == 0 ?
    CQNewGLCamera::RotateAt::POSITION : CQNewGLCamera::RotateAt::ORIGIN);
  canvas()->update();
}

void
CQNewGLCameraControl::
strafeSlot(int b)
{
  camera()->setStrafe(b);
  canvas()->update();
}

void
CQNewGLCameraControl::
farSlot(double r)
{
  camera()->setFar(r);
  canvas()->update();
}

void
CQNewGLCameraControl::
yawSlot(double r)
{
  camera()->setYaw(r);
  canvas()->update();
}

void
CQNewGLCameraControl::
pitchSlot(double r)
{
  camera()->setPitch(r);
  canvas()->update();
}

void
CQNewGLCameraControl::
rollSlot(double r)
{
  camera()->setRoll(r);
  canvas()->update();
}

void
CQNewGLCameraControl::
speedSlot(double r)
{
  camera()->setMovementSpeed(r);
  canvas()->update();
}

void
CQNewGLCameraControl::
originSlot()
{
  auto p = originEdit_->getValue();

  camera()->setOrigin(CGLVector3D(p.x, p.y, p.z));

  canvas()->updateCameraBuffer();
  canvas()->update();
}

void
CQNewGLCameraControl::
positionSlot()
{
  auto p = positionEdit_->getValue();

  camera()->setPosition(CGLVector3D(p.x, p.y, p.z));

  canvas()->updateCameraBuffer();
  canvas()->update();
}

void
CQNewGLCameraControl::
upSlot()
{
  auto p = upEdit_->getValue();

  camera()->setUp(CGLVector3D(p.x, p.y, p.z));

  canvas()->updateCameraBuffer();
  canvas()->update();
}

void
CQNewGLCameraControl::
rightSlot()
{
  auto p = rightEdit_->getValue();

  camera()->setRight(CGLVector3D(p.x, p.y, p.z));

  canvas()->updateCameraBuffer();
  canvas()->update();
}

void
CQNewGLCameraControl::
colorSlot(const QColor &c)
{
  camera()->setColor(c);

  canvas()->updateCameraBuffer();
  canvas()->update();
}

void
CQNewGLCameraControl::
addSlot()
{
  auto *canvas = this->canvas();

  canvas->addCamera();

  control_->updateCameras();

  canvas->update();
}

void
CQNewGLCameraControl::
resetSlot()
{
  camera()->reset();
  canvas()->update();
}

void
CQNewGLCameraControl::
initSlot()
{
  canvas()->resetCamera();
  canvas()->update();
}

void
CQNewGLCameraControl::
invalidateCameras()
{
  camerasInvalid_ = true;
}

void
CQNewGLCameraControl::
updateWidgets()
{
  auto *canvas = this->canvas();
  auto *camera = this->camera();

  connectSlots(false);

  //---

  if (camerasInvalid_) {
    camerasInvalid_ = false;

    camerasList_->clear();

    int ind = 0;

    for (auto *camera : canvas->cameras()) {
      const auto &name = camera->name();

      auto *item = new QListWidgetItem(name);

      camerasList_->addItem(item);

      item->setData(Qt::UserRole, ind++);
    }
  }

  //---

  showCheck_->setChecked(canvas->isShowCameras());

  followCheck_->setChecked(camera->isFollowObject());

  eyelineCheck_->setChecked(canvas->isShowEyeline());

  orthoCheck_->setChecked(canvas->isOrtho());

  updateCurrentCheck_->setChecked(updateCurrent_);

  rotateCheck_->setChecked(camera->isRotate());

  rotateAtCombo_->setCurrentIndex(camera->rotateAt() == CQNewGLCamera::RotateAt::POSITION ? 0 : 1);

  strafeCheck_->setChecked(camera->isStrafe());

  zoomEdit_->setValue(camera->zoom());

  nearEdit_->setValue(camera->near());
  farEdit_ ->setValue(camera->far());

  yawEdit_  ->setValue(camera->yaw());
  pitchEdit_->setValue(camera->pitch());
  rollEdit_ ->setValue(camera->roll());

  speedEdit_ ->setValue(camera->movementSpeed());

  auto o = camera->origin();
  originEdit_->setValue(CPoint3D(o.x(), o.y(), o.z()));

  auto p = camera->position();
  positionEdit_->setValue(CPoint3D(p.x(), p.y(), p.z()));

  auto u = camera->up();
  upEdit_->setValue(CPoint3D(u.x(), u.y(), u.z()));

  auto r = camera->right();
  rightEdit_->setValue(CPoint3D(r.x(), r.y(), r.z()));

  //---

  matrixEdit_->setValue(camera->getViewMatrix().toCMatrix());

  colorEdit_->setColor(camera->color());

  //---

  connectSlots(true);
}

//---

CQNewGLNormalsControl::
CQNewGLNormalsControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  auto addLabelEdit = [&](const QString &label, auto *w) {
    auto *frame   = new QFrame;
    auto *layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);

    return w;
  };

  //---

  show1Check_ = addLabelEdit("Show Normals", new QCheckBox);
  show2Check_ = addLabelEdit("New Normals" , new QCheckBox);

  //---

  sizeEdit_  = addLabelEdit("Size" , new CQRealSpin);
  colorEdit_ = addLabelEdit("Color", new CQColorEdit(this));

  //---

  tangentSpaceCheck_ = addLabelEdit("Tangent Space", new QCheckBox(this));

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLNormalsControl::
connectSlots(bool b)
{
  if (b) {
    connect(show1Check_, SIGNAL(stateChanged(int)), this, SLOT(show1Slot(int)));
    connect(show2Check_, SIGNAL(stateChanged(int)), this, SLOT(show2Slot(int)));
    connect(sizeEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLNormalsControl::sizeSlot);
    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(colorSlot(const QColor &)));
    connect(tangentSpaceCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(tangentSpaceSlot(int)));
  }
  else {
    disconnect(show1Check_, SIGNAL(stateChanged(int)), this, SLOT(show1Slot(int)));
    disconnect(show2Check_, SIGNAL(stateChanged(int)), this, SLOT(show2Slot(int)));
    disconnect(sizeEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLNormalsControl::sizeSlot);
    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(colorSlot(const QColor &)));
    disconnect(tangentSpaceCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(tangentSpaceSlot(int)));
  }
}

void
CQNewGLNormalsControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas  = this->canvas();
  auto *normals = canvas->getNormals();

  show1Check_->setChecked(canvas->isShowNormals());
  show2Check_->setChecked(normals->isVisible());

  sizeEdit_ ->setValue(canvas->normalsSize());
  colorEdit_->setColor(canvas->normalsColor());

  tangentSpaceCheck_->setChecked(canvas->isTangentSpaceNormal());

  connectSlots(true);
}

void
CQNewGLNormalsControl::
show1Slot(int state)
{
  auto *canvas = this->canvas();

  canvas->setShowNormals(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLNormalsControl::
show2Slot(int state)
{
  auto *canvas  = this->canvas();
  auto *normals = canvas->getNormals();

  normals->setVisible(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLNormalsControl::
sizeSlot(double s)
{
  auto *canvas  = this->canvas();
  auto *normals = canvas->getNormals();

  canvas ->setNormalsSize(s);
  normals->setLineSize(s);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLNormalsControl::
colorSlot(const QColor &c)
{
  auto *canvas  = this->canvas();
  auto *normals = canvas->getNormals();

  canvas ->setNormalsColor(c);
  normals->setLineColor(c);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLNormalsControl::
tangentSpaceSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setTangentSpaceNormal(state);

  canvas->updateObjectsData();
  canvas->update();
}

//---

CQNewGLLightsControl::
CQNewGLLightsControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  auto *materialGroup  = new QGroupBox("Material");
  auto *materialLayout = new QVBoxLayout(materialGroup);

  layout->addWidget(materialGroup);

  //---

  auto addMaterialLabelEdit = [&](const QString &label, QWidget *w) {
    auto *frame   = new QFrame;
    auto *layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    materialLayout->addWidget(frame);
  };

  //---

  ambientEdit_ = new CQColorEdit(this);

  addMaterialLabelEdit("Ambient", ambientEdit_);

  //---

  diffuseEdit_ = new CQColorEdit(this);

  addMaterialLabelEdit("Diffuse", diffuseEdit_);

  //---

  emissionEdit_ = new CQColorEdit(this);

  addMaterialLabelEdit("Emission", emissionEdit_);

  //---

  auto createSlider = [&](const QString &label, double min, double max, double value,
                          const char *slotName) {
    auto *slider = new CQRealSlider(Qt::Horizontal);

    slider->setRange(min, max);
    slider->setSingleStep((max - min)/100.0);
    slider->setPageStep((max - min)/10.0);

    slider->setLabel(label);
    slider->setToolTip(label);
    slider->setValue(value);

    connect(slider, SIGNAL(valueChanged(double)), this, slotName);

    return slider;
  };

  auto *app = control_->app();

  auto *ambientSlider  = createSlider("Ambient" , 0.0, 1.0, app->ambientStrength(),
                                      SLOT(ambientFactorSlot(double)));
  auto *diffuseSlider  = createSlider("Diffuse" , 0.0, 1.0, app->diffuseStrength(),
                                      SLOT(diffuseFactorSlot(double)));
  auto *specularSlider = createSlider("Specular", 0.0, 1.0, app->specularStrength(),
                                      SLOT(specularFactorSlot(double)));
  auto *emissiveSlider = createSlider("Emissive", 0.0, 1.0, app->emissiveStrength(),
                                      SLOT(emissiveFactorSlot(double)));

  auto *shininessSlider = createSlider("Shininess", 0.0, 100.0, app->shininess(),
                                       SLOT(shininessFactorSlot(double)));

  materialLayout->addWidget(ambientSlider);
  materialLayout->addWidget(diffuseSlider);
  materialLayout->addWidget(specularSlider);
  materialLayout->addWidget(emissiveSlider);
  materialLayout->addWidget(shininessSlider);

  //---

  lightsList_ = new QListWidget;

  lightsList_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(lightsList_);

  //---

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QGridLayout(controlFrame);

  layout->addWidget(controlFrame);

  int row = 0;

  auto addLabelEdit = [&](const QString &label, QWidget *w) {
    controlLayout->addWidget(new QLabel(label), row, 0);
    controlLayout->addWidget(w, row, 1);
    ++row;
  };

  //---

  showCheck_ = new QCheckBox;
  addLabelEdit("Show", showCheck_);

  //---

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() << "Directional" << "Point" << "Spot");
  addLabelEdit("Type", typeCombo_);

  enabledCheck_ = new QCheckBox;
  addLabelEdit("Enabled", enabledCheck_);

  positionEdit_ = new CQPoint3DEdit;
  addLabelEdit("Position", positionEdit_);

  directionEdit_ = new CQPoint3DEdit;
  addLabelEdit("Direction", directionEdit_);

  colorEdit_ = new CQColorEdit;
  addLabelEdit("Color", colorEdit_);

  radiusEdit_ = new CQRealSpin;
  addLabelEdit("Radius", radiusEdit_);

  cutoffEdit_ = new CQRealSpin;
  addLabelEdit("Cut Off", cutoffEdit_);

  //---

  controlLayout->setRowStretch(row, 1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *addButton = new QPushButton("Add");

  connect(addButton, &QPushButton::clicked, this, &CQNewGLLightsControl::addSlot);

  buttonsLayout->addWidget(addButton);
  buttonsLayout->addStretch(1);
}

void
CQNewGLLightsControl::
invalidateLights()
{
  lightsInvalid_ = true;
}

void
CQNewGLLightsControl::
connectSlots(bool b)
{
  if (b) {
    connect(ambientEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(ambientSlot(const QColor &)));
    connect(diffuseEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(diffuseSlot(const QColor &)));
    connect(emissionEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(emissionSlot(const QColor &)));

    connect(lightsList_, &QListWidget::currentItemChanged,
            this, &CQNewGLLightsControl::lightSelectedSlot);

    connect(showCheck_, &QCheckBox::stateChanged, this,
            &CQNewGLLightsControl::showSlot);

    connect(typeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(typeSlot(int)));
    connect(enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot(int)));
    connect(positionEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLLightsControl::positionSlot);
    connect(directionEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLLightsControl::directionSlot);
    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(colorSlot(const QColor &)));
    connect(radiusEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLLightsControl::radiusSlot);
    connect(cutoffEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLLightsControl::cutoffSlot);
  }
  else {
    disconnect(ambientEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(ambientSlot(const QColor &)));
    disconnect(diffuseEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(diffuseSlot(const QColor &)));
    disconnect(emissionEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(emissionSlot(const QColor &)));

    disconnect(lightsList_, &QListWidget::currentItemChanged,
               this, &CQNewGLLightsControl::lightSelectedSlot);

    disconnect(showCheck_, &QCheckBox::stateChanged, this,
               &CQNewGLLightsControl::showSlot);

    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(typeSlot(int)));
    disconnect(enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot(int)));
    disconnect(positionEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLLightsControl::positionSlot);
    disconnect(directionEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLLightsControl::directionSlot);
    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(colorSlot(const QColor &)));
    disconnect(radiusEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLLightsControl::radiusSlot);
    disconnect(cutoffEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLLightsControl::cutoffSlot);
  }
}

void
CQNewGLLightsControl::
ambientSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setAmbientColor(c);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLLightsControl::
diffuseSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setDiffuseColor(c);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLLightsControl::
emissionSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setEmissionColor(c);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLLightsControl::
ambientFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setAmbientStrength(v);
  canvas->update();
}

void
CQNewGLLightsControl::
diffuseFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setDiffuseStrength(v);
  canvas->update();
}

void
CQNewGLLightsControl::
specularFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setSpecularStrength(v);
  canvas->update();
}

void
CQNewGLLightsControl::
emissiveFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setEmissiveStrength(v);
  canvas->update();
}

void
CQNewGLLightsControl::
shininessFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setShininess(v);

  canvas->updateObjectsData();
  canvas->update();
}


void
CQNewGLLightsControl::
lightSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = this->canvas();

  int ind = item->data(Qt::UserRole).toInt();

  canvas->setCurrentLight(ind);

  updateWidgets();

  canvas->update();
}

void
CQNewGLLightsControl::
showSlot(int state)
{
  auto *canvas = this->canvas();
  canvas->setShowLights(state);

  canvas->update();
}

void
CQNewGLLightsControl::
typeSlot(int ind)
{
  auto *canvas = this->canvas();
  auto *light  = canvas->getCurrentLight();

  if      (ind == 0)
    light->setType(CQNewGLLight::Type::DIRECTIONAL);
  else if (ind == 1)
    light->setType(CQNewGLLight::Type::POINT);
  else if (ind == 2)
    light->setType(CQNewGLLight::Type::SPOT);

  canvas->update();
}

void
CQNewGLLightsControl::
enabledSlot(int state)
{
  auto *canvas = this->canvas();
  auto *light  = canvas->getCurrentLight();

  light->setEnabled(state);
  canvas->update();
}

void
CQNewGLLightsControl::
positionSlot()
{
  auto *canvas = this->canvas();
  auto *light  = canvas->getCurrentLight();

  auto p = positionEdit_->getValue();

  light->setPosition(CGLVector3D(p.x, p.y, p.z));
  canvas->update();
}

void
CQNewGLLightsControl::
directionSlot()
{
  auto *canvas = this->canvas();
  auto *light  = canvas->getCurrentLight();

  auto p = directionEdit_->getValue();

  light->setDirection(CGLVector3D(p.x, p.y, p.z));
  canvas->update();
}

void
CQNewGLLightsControl::
colorSlot(const QColor &c)
{
  auto *canvas = this->canvas();
  auto *light  = canvas->getCurrentLight();

  light->setColor(c);

  canvas->updateLightBuffer();
  canvas->update();
}

void
CQNewGLLightsControl::
radiusSlot(double r)
{
  auto *canvas = this->canvas();
  auto *light  = canvas->getCurrentLight();

  light->setRadius(r);
  canvas->update();
}

void
CQNewGLLightsControl::
cutoffSlot(double r)
{
  auto *canvas = this->canvas();
  auto *light  = canvas->getCurrentLight();

  light->setCutoff(r);
  canvas->update();
}

void
CQNewGLLightsControl::
addSlot()
{
  auto *canvas = this->canvas();

  canvas->addLight();

  control_->updateLights();

  canvas->update();
}

void
CQNewGLLightsControl::
updateWidgets()
{
  auto *canvas = this->canvas();

  connectSlots(false);

  //---

  ambientEdit_ ->setColor(canvas->ambientColor());
  diffuseEdit_ ->setColor(canvas->diffuseColor());
  emissionEdit_->setColor(canvas->emissionColor());

  if (lightsInvalid_) {
    lightsInvalid_ = false;

    lightsList_->clear();

    int ind = 0;

    for (auto *light : canvas->lights()) {
      const auto &name = light->name();

      auto *item = new QListWidgetItem(name);

      lightsList_->addItem(item);

      item->setData(Qt::UserRole, ind++);
    }
  }

  //---

  showCheck_->setChecked(canvas->isShowLights());

  //---

  auto *light = canvas->getCurrentLight();

  typeCombo_    ->setCurrentIndex(static_cast<int>(light->type()));
  enabledCheck_ ->setChecked(light->isEnabled());
  positionEdit_ ->setValue(light->position().point());
  directionEdit_->setValue(light->direction().point());
  colorEdit_    ->setColor(light->color());
  radiusEdit_   ->setValue(light->radius());
  cutoffEdit_   ->setValue(light->cutoff());

  //---

  connectSlots(true);
}

//---

CQNewGLAxesControl::
CQNewGLAxesControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  axisCheck_ = new QCheckBox("Show Axis");

  layout->addWidget(axisCheck_);

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLAxesControl::
connectSlots(bool b)
{
  if (b) {
    connect(axisCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
  }
  else {
    disconnect(axisCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
  }
}

void
CQNewGLAxesControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();
  auto *axes   = canvas->getAxes();

  axisCheck_->setChecked(axes->isVisible());

  connectSlots(true);
}

void
CQNewGLAxesControl::
showSlot(int state)
{
  auto *canvas = this->canvas();
  auto *axes   = canvas->getAxes();

  axes->setVisible(state);
  canvas->update();
}

//---

CQNewGLBBoxControl::
CQNewGLBBoxControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  showCheck_ = new QCheckBox("Show BBox");

  layout->addWidget(showCheck_);

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLBBoxControl::
connectSlots(bool b)
{
  if (b) {
    connect(showCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
  }
  else {
    disconnect(showCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
  }
}

void
CQNewGLBBoxControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();
  auto *bbox   = canvas->getBBox();

  showCheck_->setChecked(bbox->isVisible());

  connectSlots(true);
}

void
CQNewGLBBoxControl::
showSlot(int state)
{
  auto *canvas = this->canvas();
  auto *bbox   = canvas->getBBox();

  bbox->setVisible(state);

  canvas->updateObjectsData();
  canvas->update();
}

//---

CQNewGLHullControl::
CQNewGLHullControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  showCheck_ = new QCheckBox("Show Hull");

  layout->addWidget(showCheck_);

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLHullControl::
connectSlots(bool b)
{
  if (b) {
    connect(showCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
  }
  else {
    disconnect(showCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
  }
}

void
CQNewGLHullControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();
  auto *hull   = canvas->getHull();

  showCheck_->setChecked(hull->isVisible());

  connectSlots(true);
}

void
CQNewGLHullControl::
showSlot(int state)
{
  auto *canvas = this->canvas();
  auto *hull   = canvas->getHull();

  hull->setVisible(state);

  canvas->updateObjectsData();
  canvas->update();
}

//---

CQNewGLBasisControl::
CQNewGLBasisControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  auto addLabelEdit = [&](const QString &label, auto *w) {
    auto *layout1 = new QHBoxLayout;

    layout->addLayout(layout1);

    auto *labelW = new QLabel(label);

    layout1->addWidget(labelW);
    layout1->addWidget(w);

    return w;
  };

  showCheck_ = addLabelEdit("Show Basis", new QCheckBox);

  sizeEdit_  = addLabelEdit("Size" , new CQRealSpin);
  widthEdit_ = addLabelEdit("Width", new CQRealSpin);

  basisUEdit_ = addLabelEdit("U (Right)"  , new CQPoint3DEdit);
  basisVEdit_ = addLabelEdit("V (Up)"     , new CQPoint3DEdit);
  basisWEdit_ = addLabelEdit("W (Forward)", new CQPoint3DEdit);

  layout->addStretch(1);

  //---

  connectSlots(true);

  //---

  updateWidgets();
}

void
CQNewGLBasisControl::
connectSlots(bool b)
{
  if (b) {
    connect(canvas(), SIGNAL(currentObjectChanged()), this, SLOT(updateWidgets()));

    connect(showCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
    connect(sizeEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLBasisControl::sizeSlot);
    connect(widthEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLBasisControl::widthSlot);

    connect(basisUEdit_, SIGNAL(editingFinished()), this, SLOT(basisUSlot()));
    connect(basisVEdit_, SIGNAL(editingFinished()), this, SLOT(basisVSlot()));
    connect(basisWEdit_, SIGNAL(editingFinished()), this, SLOT(basisWSlot()));
  }
  else {
    disconnect(canvas(), SIGNAL(currentObjectChanged()), this, SLOT(updateWidgets()));

    disconnect(showCheck_, SIGNAL(stateChanged(int)), this, SLOT(showSlot(int)));
    disconnect(sizeEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLBasisControl::sizeSlot);
    disconnect(widthEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLBasisControl::widthSlot);

    disconnect(basisUEdit_, SIGNAL(editingFinished()), this, SLOT(basisUSlot()));
    disconnect(basisVEdit_, SIGNAL(editingFinished()), this, SLOT(basisVSlot()));
    disconnect(basisWEdit_, SIGNAL(editingFinished()), this, SLOT(basisWSlot()));
  }
}

void
CQNewGLBasisControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();

  showCheck_->setChecked(canvas->isShowBasis());
  sizeEdit_ ->setValue(canvas->basisLineSize());
  widthEdit_->setValue(canvas->basisLineWidth());

  auto *object = canvas->getCurrentObject();

  if (object) {
    CVector3D u, v, w;
    object->getBasis(u, v, w);

    basisUEdit_->setValue(CPoint3D(u.getX(), u.getY(), u.getZ()));
    basisVEdit_->setValue(CPoint3D(v.getX(), v.getY(), v.getZ()));
    basisWEdit_->setValue(CPoint3D(w.getX(), w.getY(), w.getZ()));
  }
  else {
    basisUEdit_->setValue(CPoint3D(1, 0, 0));
    basisVEdit_->setValue(CPoint3D(0, 1, 0));
    basisWEdit_->setValue(CPoint3D(0, 0, 1));

  }

  connectSlots(true);
}

void
CQNewGLBasisControl::
showSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setShowBasis(state);
  canvas->update();
}

void
CQNewGLBasisControl::
sizeSlot(double s)
{
  auto *canvas = this->canvas();

  canvas->setBasisLineSize(s);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLBasisControl::
widthSlot(double w)
{
  auto *canvas = this->canvas();

  canvas->setBasisLineWidth(w);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLBasisControl::
basisUSlot()
{
  auto *canvas = this->canvas();

  auto *object = canvas->getCurrentObject();
  if (! object) return;

  auto u1 = basisUEdit_->getValue();
  auto v1 = basisVEdit_->getValue();
  auto w1 = basisWEdit_->getValue();

  object->setBasis(CVector3D(u1), CVector3D(v1), CVector3D(w1));

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLBasisControl::
basisVSlot()
{
  basisUSlot();
}

void
CQNewGLBasisControl::
basisWSlot()
{
  basisUSlot();
}

//---

CQNewGLObjectsControl::
CQNewGLObjectsControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  auto *groupBox    = new QGroupBox("Objects");
  auto *groupLayout = new QVBoxLayout(groupBox);

  objectsList_ = new CQNewGLObjectsList(control);

  groupLayout->addWidget(objectsList_);

  layout->addWidget(groupBox);

  //---

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QHBoxLayout(controlFrame);

  auto addControlSpacer = [&](int w) {
    auto *spacer = new QFrame;
    spacer->setFixedWidth(w);
    controlLayout->addWidget(spacer);
  };

  selectButton_   = new QPushButton("Select");
  deselectButton_ = new QPushButton("Deselect");

  controlLayout->addWidget(selectButton_);
  controlLayout->addWidget(deselectButton_);

  addControlSpacer(8);

  showButton_ = new QPushButton("Show");
  hideButton_ = new QPushButton("Hide");

  showButton_->setEnabled(false);
  hideButton_->setEnabled(false);

  controlLayout->addWidget(showButton_);
  controlLayout->addWidget(hideButton_);

  addControlSpacer(8);

  currentButton_ = new QPushButton("Current");

  currentButton_->setEnabled(false);

  controlLayout->addWidget(currentButton_);

  controlLayout->addStretch(1);

  groupLayout->addWidget(controlFrame);

  //---

  auto addLabelEdit = [&](const QString &label, auto *w) {
    auto *layout1 = new QHBoxLayout;

    layout->addLayout(layout1);

    auto *labelW = new QLabel(label);

    layout1->addWidget(labelW);
    layout1->addWidget(w);

    return w;
  };

  auto createTextureEdit = [&](const QString &label) {
    auto *edit = new CQNewGLTextureChooser(control_);

    addLabelEdit(label, edit);

    return edit;
  };

#if 0
  auto createStringEdit = [&](const QString &label) {
    auto *edit = new QLineEdit;

    addLabelEdit(label, edit);

    return edit;
  };
#endif

  //---

  centerEdit_ = addLabelEdit("Center", new CQPoint3DEdit);
  sizeEdit_   = addLabelEdit("Size"  , new CQPoint3DEdit);

#if 0
  translateEdit_ = addLabelEdit("Translate", new CQPoint3DEdit);
  rotateEdit_    = addLabelEdit("Rotate"   , new CQPoint3DEdit);
#endif

  //---

  diffuseTextureEdit_  = createTextureEdit("Diffuse Map" );
  normalTextureEdit_   = createTextureEdit("Normal Map"  );
  specularTextureEdit_ = createTextureEdit("Specular Map");
  emissiveTextureEdit_ = createTextureEdit("Emissive Map");

  //---

  layout->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *addButton   = new QPushButton("Add");
  auto *resetButton = new QPushButton("Reset Transform");

  connect(addButton, &QPushButton::clicked, this, &CQNewGLObjectsControl::addSlot);
  connect(resetButton, &QPushButton::clicked, this, &CQNewGLObjectsControl::resetSlot);

  buttonsLayout->addWidget(addButton);
  buttonsLayout->addWidget(resetButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLObjectsControl::
updateWidgets()
{
  connectSlots(false);

  //---

  objectsList_->updateObjects();

  //---

  connectSlots(true);
}

void
CQNewGLObjectsControl::
connectSlots(bool b)
{
  if (b) {
    connect(canvas(), SIGNAL(modelMatrixChanged()), this, SLOT(updateModelMatrix()));

    connect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
            this, &CQNewGLObjectsControl::objectSelectedSlot);

    connect(selectButton_  , &QPushButton::clicked, this, &CQNewGLObjectsControl::selectSlot);
    connect(deselectButton_, &QPushButton::clicked, this, &CQNewGLObjectsControl::deselectSlot);
    connect(showButton_    , &QPushButton::clicked, this, &CQNewGLObjectsControl::showSlot);
    connect(hideButton_    , &QPushButton::clicked, this, &CQNewGLObjectsControl::hideSlot);
    connect(currentButton_ , &QPushButton::clicked, this, &CQNewGLObjectsControl::currentSlot);

    connect(centerEdit_, SIGNAL(editingFinished()), this, SLOT(centerSlot()));
    connect(sizeEdit_  , SIGNAL(editingFinished()), this, SLOT(sizeSlot()));

#if 0
    connect(translateEdit_, SIGNAL(editingFinished()), this, SLOT(translateSlot()));
    connect(rotateEdit_, SIGNAL(editingFinished()), this, SLOT(rotateSlot()));
#endif

    connect(diffuseTextureEdit_ , SIGNAL(textureChanged()), this, SLOT(diffuseMapSlot()));
    connect(normalTextureEdit_  , SIGNAL(textureChanged()), this, SLOT(normalMapSlot()));
    connect(specularTextureEdit_, SIGNAL(textureChanged()), this, SLOT(specularMapSlot()));
    connect(emissiveTextureEdit_, SIGNAL(textureChanged()), this, SLOT(emissiveMapSlot()));
  }
  else {
    disconnect(canvas(), SIGNAL(modelMatrixChanged()), this, SLOT(updateModelMatrix()));

    disconnect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
               this, &CQNewGLObjectsControl::objectSelectedSlot);

    disconnect(selectButton_  , &QPushButton::clicked, this, &CQNewGLObjectsControl::selectSlot);
    disconnect(deselectButton_, &QPushButton::clicked, this, &CQNewGLObjectsControl::deselectSlot);
    disconnect(showButton_    , &QPushButton::clicked, this, &CQNewGLObjectsControl::showSlot);
    disconnect(hideButton_    , &QPushButton::clicked, this, &CQNewGLObjectsControl::hideSlot);
    disconnect(currentButton_ , &QPushButton::clicked, this, &CQNewGLObjectsControl::currentSlot);

    disconnect(centerEdit_, SIGNAL(editingFinished()), this, SLOT(centerSlot()));
    disconnect(sizeEdit_  , SIGNAL(editingFinished()), this, SLOT(sizeSlot()));

#if 0
    disconnect(translateEdit_, SIGNAL(editingFinished()), this, SLOT(translateSlot()));
    disconnect(rotateEdit_, SIGNAL(editingFinished()), this, SLOT(rotateSlot()));
#endif

    disconnect(diffuseTextureEdit_ , SIGNAL(textureChanged()), this, SLOT(diffuseMapSlot()));
    disconnect(normalTextureEdit_  , SIGNAL(textureChanged()), this, SLOT(normalMapSlot()));
    disconnect(specularTextureEdit_, SIGNAL(textureChanged()), this, SLOT(specularMapSlot()));
    disconnect(emissiveTextureEdit_, SIGNAL(textureChanged()), this, SLOT(emissiveMapSlot()));
  }
}

void
CQNewGLObjectsControl::
updateModelMatrix()
{
  connectSlots(false);

#if 0
  auto tv = canvas()->modelTranslate();
  auto rv = canvas()->modelRotate();

  translateEdit_->setValue(CPoint3D(tv.x(), tv.y(), tv.z()));
  rotateEdit_   ->setValue(CPoint3D(rv.x(), rv.y(), rv.z()));
#endif

  connectSlots(true);
}

void
CQNewGLObjectsControl::
objectSelectedSlot()
{
  int ind = objectsList_->selectedInd();

  updateSelected(ind);
}

void
CQNewGLObjectsControl::
updateSelected(int ind)
{
  auto *object = canvas()->getObject(ind);

  //---

  connectSlots(false);

  //---

  selectButton_  ->setEnabled(ind >= 0 && ! object->getSelected());
  deselectButton_->setEnabled(ind >= 0 &&   object->getSelected());

  showButton_->setEnabled(object && ! object->getVisible());
  hideButton_->setEnabled(object &&   object->getVisible());

  currentButton_->setEnabled(object);

  //---

  if (object) {
    auto bbox = canvas()->getObjectBBox(object);

    centerEdit_->setValue(bbox.getCenter());
    sizeEdit_  ->setValue(bbox.getSize().point());
  }
  else {
    centerEdit_->setValue(CPoint3D(0, 0, 0));
    sizeEdit_  ->setValue(CPoint3D(1, 1, 1));
  }

  //---

  auto *textureMap  = (object ? object->getDiffuseTexture () : nullptr);
  auto *normalMap   = (object ? object->getNormalTexture  () : nullptr);
  auto *specularMap = (object ? object->getSpecularTexture() : nullptr);
  auto *emissiveMap = (object ? object->getEmissiveTexture() : nullptr);

  diffuseTextureEdit_ ->setCurrentText(
    textureMap  ? QString::fromStdString(textureMap ->name()) : "");
  normalTextureEdit_  ->setCurrentText(
    normalMap   ? QString::fromStdString(normalMap  ->name()) : "");
  specularTextureEdit_->setCurrentText(
    specularMap ? QString::fromStdString(specularMap->name()) : "");
  emissiveTextureEdit_->setCurrentText(
    emissiveMap ? QString::fromStdString(emissiveMap->name()) : "");

  canvas()->update();
  uvMap ()->update();

  //---

  connectSlots(true);

  //---

  updateModelMatrix();
}

void
CQNewGLObjectsControl::
selectSlot()
{
  auto *object = getObjectListSelected();

  if (object) {
    object->setSelected(true);

    canvas()->update();

    selectButton_  ->setEnabled(false);
    deselectButton_->setEnabled(true);
  }
}

void
CQNewGLObjectsControl::
deselectSlot()
{
  auto *object = getObjectListSelected();

  if (object) {
    object->setSelected(false);

    canvas()->update();

    selectButton_  ->setEnabled(true);
    deselectButton_->setEnabled(false);
  }
}

void
CQNewGLObjectsControl::
showSlot()
{
  auto *object = getObjectListSelected();

  if (object) {
    object->setVisible(true);

    canvas()->updateObjectsData();
    canvas()->update();

    showButton_->setEnabled(false);
    hideButton_->setEnabled(true);
  }
}

void
CQNewGLObjectsControl::
hideSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  object->setVisible(false);

  canvas()->updateObjectsData();
  canvas()->update();

  showButton_->setEnabled(true);
  hideButton_->setEnabled(false);
}

void
CQNewGLObjectsControl::
currentSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  canvas()->setCurrentObject(object);
}

void
CQNewGLObjectsControl::
centerSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  auto bbox = canvas()->getObjectBBox(object);

  auto c1 = bbox.getCenter();
  auto c2 = centerEdit_->getValue();

  auto t = CMatrix3D::translation(c2.x - c1.x, c2.y - c1.y, c2.z - c1.z);

  object->setTransform(t*object->getTransform());

  canvas()->updateObjectsData();
  canvas()->update();

  //---

  bbox = canvas()->getObjectBBox(object);

  centerEdit_->setValue(bbox.getCenter());
  sizeEdit_  ->setValue(bbox.getSize().point());
}

void
CQNewGLObjectsControl::
sizeSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  auto bbox = canvas()->getObjectBBox(object);

  auto s1 = bbox.getSize().point();
  auto s2 = sizeEdit_->getValue();

  double f = 1.0;

  auto dx = std::abs(s2.x - s1.x);
  auto dy = std::abs(s2.y - s1.y);
  auto dz = std::abs(s2.z - s1.z);

  if      (dx > dy && dx > dz) f = s2.x/s1.x;
  else if (dy > dz)            f = s2.y/s1.y;
  else                         f = s2.z/s1.z;

  auto s = CMatrix3D::scale(f, f, f);

  object->setTransform(s*object->getTransform());

  canvas()->updateObjectsData();
  canvas()->update();

  //---

  bbox = canvas()->getObjectBBox(object);

  centerEdit_->setValue(bbox.getCenter());
  sizeEdit_  ->setValue(bbox.getSize().point());
}

void
CQNewGLObjectsControl::
translateSlot()
{
//auto p = translateEdit_->getValue();
//canvas()->setModelTranslate(CVector3D(p.x, p.y, p.z));

  canvas()->update();
}

void
CQNewGLObjectsControl::
rotateSlot()
{
//auto p = rotateEdit_->getValue();
//canvas()->setModelRotate(CVector3D(p.x, p.y, p.z));

  canvas()->update();
}

void
CQNewGLObjectsControl::
diffuseMapSlot()
{
  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas()->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setDiffuseTexture(texture);

    canvas()->updateObjectsData();
    canvas()->update();
    uvMap ()->update();
  }
}

void
CQNewGLObjectsControl::
normalMapSlot()
{
  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas()->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setNormalTexture(texture);

    canvas()->updateObjectsData();
    canvas()->update();
  }
}

void
CQNewGLObjectsControl::
specularMapSlot()
{
  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas()->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setSpecularTexture(texture);

    canvas()->updateObjectsData();
    canvas()->update();
  }
}

void
CQNewGLObjectsControl::
emissiveMapSlot()
{
  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas()->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setEmissiveTexture(texture);

    canvas()->updateObjectsData();
    canvas()->update();
  }
}

void
CQNewGLObjectsControl::
addSlot()
{
//auto dir = QDir::current().dirName();
  auto dir = canvas()->app()->buildDir() + "/models";

  auto fileName = QFileDialog::getOpenFileName(this, "Open Model File", dir, "Files (*.*)");
  if (! fileName.length()) return;

  auto format = CImportBase::filenameToType(fileName.toStdString());

  CQNewGLCanvas::LoadData loadData;

  if (! canvas()->loadModel(fileName, format, loadData))
    std::cerr << "Failed to load model '" << fileName.toStdString() << "'\n";

  canvas()->updateModelData();
  canvas()->update();
}

void
CQNewGLObjectsControl::
resetSlot()
{
  auto *object = getObjectListSelected();

  if (object) {
    object->setTransform(CMatrix3D::identity());
    canvas()->update();
  }
}

CGeomObject3D *
CQNewGLObjectsControl::
getSelectedObject() const
{
  return canvas()->getSelectedObject();
}

CGeomObject3D *
CQNewGLObjectsControl::
getObjectListSelected() const
{
  return objectsList_->getObjectListSelected();
}

//---

CQNewGLTexturesControl::
CQNewGLTexturesControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  texturesList_ = new QListWidget;

  texturesList_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(texturesList_);

  //---

  image_ = new CQNewGLTextureImage(this);

  image_->setFixedSize(QSize(256, 256));

  layout->addWidget(image_);

  //---

  layout->addStretch(1);

  //---

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QHBoxLayout(controlFrame);

  layout->addWidget(controlFrame);

  flipCheck_  = new QCheckBox("Flip");
  loadButton_ = new QPushButton("Load");

  controlLayout->addWidget(flipCheck_);
  controlLayout->addWidget(loadButton_);
  controlLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLTexturesControl::
invalidateTextures()
{
  texturesInvalid_ = true;
}

void
CQNewGLTexturesControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();

  if (texturesInvalid_) {
    texturesInvalid_ = false;

    texturesList_->clear();

    int ind = 0;

    const auto &textures = canvas->glTextures();

    for (const auto &pr : textures) {
      const auto &textureData = pr.second;

      auto objectName = QString::fromStdString(textureData.glTexture->getName());

      auto *item = new QListWidgetItem(objectName);

      texturesList_->addItem(item);

      item->setData(Qt::UserRole, ind++);
    }
  }

  connectSlots(true);
}

void
CQNewGLTexturesControl::
connectSlots(bool b)
{
  if (b) {
    connect(texturesList_, &QListWidget::currentItemChanged,
            this, &CQNewGLTexturesControl::textureSelectedSlot);

  //connect(flipCheck_, &QCheckBox::stateChanged, this, &CQNewGLTexturesControl::flipSlot);
    connect(loadButton_, &QPushButton::clicked, this, &CQNewGLTexturesControl::loadSlot);
  }
  else {
    disconnect(texturesList_, &QListWidget::currentItemChanged,
               this, &CQNewGLTexturesControl::textureSelectedSlot);

  //disconnect(flipCheck_, &QCheckBox::stateChanged, this, &CQNewGLTexturesControl::flipSlot);
    disconnect(loadButton_, &QPushButton::clicked, this, &CQNewGLTexturesControl::loadSlot);
  }
}

void
CQNewGLTexturesControl::
textureSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  textureInd_ = item->data(Qt::UserRole).toInt();

  image_->update();
}

QString
CQNewGLTexturesControl::
textureName() const
{
  auto *canvas = this->canvas();

  const auto &textures = canvas->glTextures();

  int ind = 0;

  for (const auto &pr : textures) {
    if (ind == textureInd_) {
      const auto &textureData = pr.second;

      return QString::fromStdString(textureData.glTexture->getName());
    }

    ++ind;
  }

  return "";
}

//---

void
CQNewGLTexturesControl::
loadSlot()
{
  auto dir = QDir::current().dirName();

  auto fileName = QFileDialog::getOpenFileName(this, "Open Texture", dir, "Files (*.*)");
  if (! fileName.length()) return;

  bool flipped = flipCheck_->isChecked();

  auto *canvas = this->canvas();

  auto *texture = canvas->getTextureByName(fileName.toStdString());

  if (! texture || texture->isFlipped() != flipped) {
    CImageFileSrc src(fileName.toStdString());

    auto image = CImageMgrInst->createImage(src);

    auto name = fileName;

    if (flipped) {
      image->flipH();

      name += " (Flipped)";
    }

    auto *texture1 = CGeometryInst->createTexture(image);

    texture1->setName(name.toStdString());

    (void) canvas->getTexture(texture1, /*add*/true);

    control_->updateWidgets();
  }
}

//---

CQNewGLControlFrame::
CQNewGLControlFrame(CQNewGLControl *control) :
 control_(control)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

CQNewGLCanvas *
CQNewGLControlFrame::
canvas() const
{
  return control_->canvas();
}

CQNewGLUVMap *
CQNewGLControlFrame::
uvMap() const
{
  return control_->uvMap();
}

//---

CQNewGLTextureImage::
CQNewGLTextureImage(CQNewGLTexturesControl *control) :
 control_(control)
{
}

void
CQNewGLTextureImage::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), Qt::white);

  auto *canvas     = control_->canvas();
  int   textureInd = control_->textureInd();

  const auto &textures = canvas->glTextures();

  auto n = int(textures.size());

  if (textureInd < 0 || textureInd >= n)
    return;

  int i = 0;

  CQGLTexture *texture = nullptr;

  for (const auto &pr : textures) {
    const auto &textureData = pr.second;

    if (i == textureInd) {
      texture = textureData.glTexture;
      break;
    }

    ++i;
  }

  if (! texture)
    return;

  const auto &qimage = texture->getImage();

  auto qimage1 = qimage.scaled(256, 256,
    Qt::KeepAspectRatio, Qt::FastTransformation);

  p.drawImage(0, 0, qimage1);
}

//---

CQNewGLTextureChooser::
CQNewGLTextureChooser(CQNewGLControl *control) :
 control_(control)
{
  connectSlots(true);

  auto *canvas = control_->canvas();

  connect(canvas, SIGNAL(textureAdded()), this, SLOT(needsUpdateSlot()));
}

void
CQNewGLTextureChooser::
needsUpdateSlot()
{
  needsUpdate_ = true;

  updateWidgets();
}

void
CQNewGLTextureChooser::
setTextureName(const QString &name)
{
  auto *canvas = control_->canvas();

  const auto &textures = canvas->glTextures();

  int ind = 0;

  for (const auto &pr : textures) {
    const auto &textureData = pr.second;

    if (QString::fromStdString(textureData.glTexture->getName()) == name) {
      setCurrentIndex(ind + 1);
      return;
    }

    ++ind;
  }

  setCurrentIndex(0);
}

void
CQNewGLTextureChooser::
updateWidgets()
{
  if (! needsUpdate_)
    return;

  connectSlots(false);

  int currentInd = currentIndex();

  auto *canvas = control_->canvas();

  clear();

  addItem("", QVariant(-1));

  int ind = 0;

  const auto &textures = canvas->glTextures();

  for (const auto &pr : textures) {
    const auto &textureData = pr.second;

    auto objectName = QString::fromStdString(textureData.glTexture->getName());

    addItem(objectName, QVariant(ind++));
  }

  setCurrentIndex(currentInd);

  connectSlots(true);
}

void
CQNewGLTextureChooser::
connectSlots(bool b)
{
  if (b)
    connect(this, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentIndexChanged(int)));
  else
    disconnect(this, SIGNAL(currentIndexChanged(int)),
               this, SLOT(currentIndexChanged(int)));
}

void
CQNewGLTextureChooser::
currentIndexChanged(int ind)
{
  auto *canvas = control_->canvas();

  textureName_ = "";

  int ind1 = 1;

  const auto &textures = canvas->glTextures();

  for (const auto &pr : textures) {
    const auto &textureData = pr.second;

    if (ind == ind1) {
      textureName_ = QString::fromStdString(textureData.glTexture->getName());
      break;
    }

    ++ind1;
  }

  Q_EMIT textureChanged();
}

//---

CQNewGLUVControl::
CQNewGLUVControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  objectsList_ = new CQNewGLObjectsList(control);

  layout->addWidget(objectsList_);

  typeCombo_ = new QComboBox;

  typeCombo_->addItems(QStringList() <<
    "None" << "Diffuse" << "Normal" << "Specular" << "Emissive");

  typeCombo_->setCurrentIndex(1);

  layout->addWidget(typeCombo_);

  layout->addStretch(1);

  connectSlots(true);
}

void
CQNewGLUVControl::
connectSlots(bool b)
{
  if (b) {
    connect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
            this, &CQNewGLUVControl::objectSelectedSlot);
    connect(typeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(typeChanged(int)));
  }
  else {
    disconnect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
               this, &CQNewGLUVControl::objectSelectedSlot);
    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(typeChanged(int)));
  }
}

void
CQNewGLUVControl::
updateObjects()
{
  connectSlots(false);

  objectsList_->updateObjects();

  connectSlots(true);
}

void
CQNewGLUVControl::
objectSelectedSlot()
{
  auto *object = objectsList_->getSelectedIndObject();

  uvMap()->setObject(object);
}

void
CQNewGLUVControl::
typeChanged(int ind)
{
  CQNewGLUVMap::TextureType textureType;

  switch (ind) {
    default:
    case 0: textureType = CQNewGLUVMap::TextureType::NONE;     break;
    case 1: textureType = CQNewGLUVMap::TextureType::DIFFUSE;  break;
    case 2: textureType = CQNewGLUVMap::TextureType::NORMAL;   break;
    case 3: textureType = CQNewGLUVMap::TextureType::SPECULAR; break;
    case 4: textureType = CQNewGLUVMap::TextureType::EMISSIVE; break;
  }

  control_->uvMap()->setTextureType(textureType);
}

//---

CQNewGLAnimControl::
CQNewGLAnimControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //---

  auto addLabelEdit = [&](const QString &label, auto *w) {
    auto *frame   = new QFrame;
    auto *layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);

    return w;
  };

  //---

  enabledCheck_ = new QCheckBox("Enabled");

  layout->addWidget(enabledCheck_);

  //---

  bonesCheck_ = new QCheckBox("Bones");

  layout->addWidget(bonesCheck_);

  bonesTransform_ = new QComboBox;
  bonesTransform_->addItems(QStringList() <<
    "Inverse Bind" << "Local" << "Global");

  addLabelEdit("Bones Transform", bonesTransform_);

  //---

  showBonePointsCheck_ = new QCheckBox("Show Bone Points");

  showBonePointsCheck_->setChecked(canvas()->isShowBonePoints());

  layout->addWidget(showBonePointsCheck_);

  //---

//objectsList_ = new CQNewGLObjectsList(control);

//layout->addWidget(objectsList_);

  //---

  nameCombo_ = new QComboBox;
  addLabelEdit("Name", nameCombo_);

  //---

  timeEdit_ = addLabelEdit("Time", new CQRealSpin);

  //---

  auto addToolButton = [&](const QString &name, const QString &iconName,
                           const QString &tip, const char *slotName) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto *buttonFrame  = new QFrame;
  auto *buttonLayout = new QHBoxLayout(buttonFrame);

  playButton_  = addToolButton("play" , "PLAY"    , "Play" , SLOT(playSlot()));
  pauseButton_ = addToolButton("pause", "PAUSE"   , "Pause", SLOT(pauseSlot()));
  stepButton_  = addToolButton("step" , "PLAY_ONE", "Step" , SLOT(stepSlot()));

  buttonLayout->addWidget(playButton_);
  buttonLayout->addWidget(pauseButton_);
  buttonLayout->addWidget(stepButton_);
  buttonLayout->addStretch(1);

  layout->addWidget(buttonFrame);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);

  //---

  connect(canvas(), SIGNAL(timerStep()), this, SLOT(timerSlot()));
}

#if 0
void
CQNewGLAnimControl::
updateObjects()
{
  connectSlots(false);

  objectsList_->updateObjects();

  connectSlots(true);
}
#endif

void
CQNewGLAnimControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = control_->canvas();

  enabledCheck_->setChecked(canvas->isAnimEnabled());
  bonesCheck_  ->setChecked(canvas->isBonesEnabled());

  if      (canvas->bonesTransform() == CQNewGLCanvas::BonesTransform::INVERSE_BIND)
    bonesTransform_->setCurrentIndex(0);
  else if (canvas->bonesTransform() == CQNewGLCanvas::BonesTransform::LOCAL)
    bonesTransform_->setCurrentIndex(1);
  else if (canvas->bonesTransform() == CQNewGLCanvas::BonesTransform::GLOBAL)
    bonesTransform_->setCurrentIndex(2);

  nameCombo_->clear();

  nameCombo_->addItem("");

  auto *object = control_->getRootObject();

  double tmin = 0.0;
  double tmax = 1.0;

  if (object) {
    std::vector<std::string> animNames;
    object->getAnimationNames(animNames);

    for (const auto &name : animNames)
      nameCombo_->addItem(QString::fromStdString(name));

    if (! animNames.empty())
      object->getAnimationRange(animNames[0], tmin, tmax);
  }

  nameCombo_->setCurrentIndex(0);

  timeEdit_->setRange(tmin, tmax);

  //---

  timeEdit_   ->setEnabled(false);
  playButton_ ->setEnabled(false);
  pauseButton_->setEnabled(false);
  stepButton_ ->setEnabled(false);

  //---

  connectSlots(true);
}

void
CQNewGLAnimControl::
connectSlots(bool b)
{
  if (b) {
//  connect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
//          this, &CQNewGLAnimControl::objectSelectedSlot);

    connect(enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot(int)));
    connect(bonesCheck_, SIGNAL(stateChanged(int)), this, SLOT(bonesSlot(int)));
    connect(showBonePointsCheck_, SIGNAL(stateChanged(int)), this, SLOT(showBonePointsSlot(int)));

    connect(bonesTransform_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(bonesTransformSlot(int)));
    connect(nameCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(nameChanged(int)));
    connect(timeEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLAnimControl::timeSlot);
  }
  else {
//  disconnect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
//             this, &CQNewGLAnimControl::objectSelectedSlot);

    disconnect(enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot(int)));
    disconnect(bonesCheck_, SIGNAL(stateChanged(int)), this, SLOT(bonesSlot(int)));
    disconnect(showBonePointsCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(showBonePointsSlot(int)));

    disconnect(bonesTransform_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(bonesTransformSlot(int)));
    disconnect(nameCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(nameChanged(int)));
    disconnect(timeEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLAnimControl::timeSlot);
  }
}

void
CQNewGLAnimControl::
enabledSlot(int state)
{
  auto *canvas = control_->canvas();

  canvas->setAnimEnabled(state);
  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnimControl::
bonesSlot(int state)
{
  auto *canvas = control_->canvas();

  canvas->setBonesEnabled(state);
  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnimControl::
showBonePointsSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setShowBonePoints(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnimControl::
bonesTransformSlot(int ind)
{
  auto *canvas = control_->canvas();

  if      (ind == 0)
    canvas->setBonesTransform(CQNewGLCanvas::BonesTransform::INVERSE_BIND);
  else if (ind == 1)
    canvas->setBonesTransform(CQNewGLCanvas::BonesTransform::LOCAL);
  else if (ind == 2)
    canvas->setBonesTransform(CQNewGLCanvas::BonesTransform::GLOBAL);

  canvas->updateObjectsData();
  canvas->update();
}

#if 0
void
CQNewGLAnimControl::
objectSelectedSlot()
{
  updateWidgets();
}
#endif

void
CQNewGLAnimControl::
nameChanged(int ind)
{
  auto name = nameCombo_->itemText(ind);

  auto *canvas = control_->canvas();

  canvas->setAnimName(name);
  canvas->updateObjectsData();
  canvas->update();

  timeEdit_   ->setEnabled(name != "");
  playButton_ ->setEnabled(name != "");
  pauseButton_->setEnabled(name != "");
  stepButton_ ->setEnabled(name != "");
}

void
CQNewGLAnimControl::
timeSlot(double t)
{
  auto *canvas = control_->canvas();

  canvas->setTime(t);

  if (canvas->isShowBonePoints())
    canvas->updateObjectsData();

  canvas->update();
}

void
CQNewGLAnimControl::
playSlot()
{
  step();

  running_ = true;
}

void
CQNewGLAnimControl::
pauseSlot()
{
  running_ = false;
}

void
CQNewGLAnimControl::
stepSlot()
{
  step();
}

void
CQNewGLAnimControl::
timerSlot()
{
  if (running_)
    step();
}

void
CQNewGLAnimControl::
step()
{
  auto min = timeEdit_->minimum();
  auto max = timeEdit_->maximum();

  auto v = timeEdit_->value();

  v += (max - min)/100.0;

  if (v >= max)
    v = min;

  timeEdit_->setValue(v);
}

//---

CQNewGLBonesControl::
CQNewGLBonesControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  //--

  bonesList_ = new QListWidget;

  bonesList_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(bonesList_);

  //---

  matrixCombo_ = new QComboBox;

  matrixCombo_->addItems(QStringList() <<
    "Inverse Bind" << "Local" << "Global" << "Anim");

  addLabelEdit("Matrix Type", matrixCombo_);

  //---

  animNameCombo_ = new QComboBox;
  addLabelEdit("Anim Name", animNameCombo_);

  //---

  animTimeEdit_ = new CQRealSpin;
  addLabelEdit("Anim Time", animTimeEdit_);

  //---

  inverseCheck_ = new QCheckBox("Invert");

  layout->addWidget(inverseCheck_);

  //---

  matrixEdit_ = new CQMatrix3D;

  addLabelEdit("Matrix", matrixEdit_, /*vertical*/true);

  //---

  connectSlots(true);
}

void
CQNewGLBonesControl::
updateWidgets()
{
  connectSlots(false);

//auto *object = getBonesObject();
  auto *object = control_->getRootObject();

  bonesList_->clear();

  if (object) {
#if 0
    const auto &nodes = object->getNodes();

    for (const auto &pn : nodes) {
      const auto &nodeId   = pn.first;
      const auto &nodeData = pn.second;

      auto objectName = QString("%1").arg(nodeId);

      objectName += QString(" %1").arg(QString::fromStdString(nodeData.name));

      if (nodeData.parent >= 0)
        objectName += QString(" Parent:%1").arg(nodeData.parent);

      if (! nodeData.children.empty()) {
        objectName += " Children:";

        int ic = 0;

        for (const auto &c : nodeData.children) {
          if (ic != 0)
            objectName += ", ";

          objectName += QString("%1").arg(c);

          ++ic;
        }
      }

      auto *item = new QListWidgetItem(objectName);

      bonesList_->addItem(item);

      item->setData(Qt::UserRole, nodeId);
    }
#else
    const auto &nodeIds = object->getNodeIds();

    for (const auto &nodeId : nodeIds) {
      const auto &nodeData = object->getNode(nodeId);
      if (! nodeData.valid) continue;

      auto objectName = QString("%1").arg(nodeId);

      objectName += QString(" %1").arg(QString::fromStdString(nodeData.name));

      if (nodeData.parent >= 0)
        objectName += QString(" Parent:%1").arg(nodeData.parent);

      if (! nodeData.children.empty()) {
        objectName += " Children:";

        int ic = 0;

        for (const auto &c : nodeData.children) {
          if (ic != 0)
            objectName += ", ";

          objectName += QString("%1").arg(c);

          ++ic;
        }
      }

      auto *item = new QListWidgetItem(objectName);

      bonesList_->addItem(item);

      item->setData(Qt::UserRole, nodeId);
    }
#endif
  }

  //---

  animNameCombo_->clear();

  auto *rootObject = control_->getRootObject();

  if (rootObject) {
    std::vector<std::string> animNames;
    rootObject->getAnimationNames(animNames);

    animNameCombo_->addItem("");

    for (const auto &name : animNames)
      animNameCombo_->addItem(QString::fromStdString(name));
  }

  connectSlots(true);
}

CGeomObject3D *
CQNewGLBonesControl::
getBonesObject() const
{
  return canvas()->getBonesObject();
}

void
CQNewGLBonesControl::
connectSlots(bool b)
{
  if (b) {
    connect(bonesList_, &QListWidget::currentItemChanged,
            this, &CQNewGLBonesControl::boneSelectedSlot);
    connect(matrixCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(matrixTypeChanged(int)));
    connect(inverseCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(inverseSlot(int)));
    connect(animNameCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(animNameChanged(int)));
    connect(animTimeEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLBonesControl::animTimeSlot);
  }
  else {
    disconnect(bonesList_, &QListWidget::currentItemChanged,
               this, &CQNewGLBonesControl::boneSelectedSlot);
    disconnect(matrixCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(matrixTypeChanged(int)));
    disconnect(inverseCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(inverseSlot(int)));
    disconnect(animNameCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(animNameChanged(int)));
    disconnect(animTimeEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLBonesControl::animTimeSlot);
  }
}

void
CQNewGLBonesControl::
boneSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  boneInd_ = item->data(Qt::UserRole).toInt();

  updateApp();
  updateMatrix();
}

void
CQNewGLBonesControl::
updateMatrix()
{
  connectSlots(false);

  auto matrix = getMatrix();

  matrixEdit_->setValue(matrix);

  connectSlots(true);
}

void
CQNewGLBonesControl::
updateApp()
{
  auto *app = control_->app();

  app->setBoneInd     (boneInd_);
  app->setBoneMatrix  (getMatrix());
  app->setBoneAnimName(boneAnimName_);
  app->setBoneAnimTime(boneAnimTime_);

  if (app->isShowBone()) {
    auto *canvas = control_->canvas();

    canvas->updateObjectsData();
    canvas->update();
  }
}

CMatrix3D
CQNewGLBonesControl::
getMatrix() const
{
//auto *object = getBonesObject();
  auto *object = control_->getRootObject();

  if (! object || boneInd_ < 0)
    return CMatrix3D::identity();

  const auto &node = object->getNode(boneInd_);

  CMatrix3D matrix;

  if      (matrixType_ == MatrixType::INVERSE_BIND)
    matrix = node.inverseBindMatrix;
  else if (matrixType_ == MatrixType::LOCAL)
    matrix = node.localTransform;
  else if (matrixType_ == MatrixType::GLOBAL)
    matrix = node.globalTransform;
  else if (matrixType_ == MatrixType::ANIM) {
    object->updateNodesAnimationData(boneAnimName_.toStdString(), boneAnimTime_);

    matrix = node.animMatrix;
  }

  if (inverse_)
    matrix = matrix.inverse();

  return matrix;
}

void
CQNewGLBonesControl::
matrixTypeChanged(int ind)
{
  if      (ind == 0)
    matrixType_ = MatrixType::INVERSE_BIND;
  else if (ind == 1)
    matrixType_ = MatrixType::LOCAL;
  else if (ind == 2)
    matrixType_ = MatrixType::GLOBAL;
  else if (ind == 3)
    matrixType_ = MatrixType::ANIM;

  updateApp();
  updateMatrix();
}

void
CQNewGLBonesControl::
inverseSlot(int state)
{
  inverse_ = state;

  updateApp();
  updateMatrix();
}

void
CQNewGLBonesControl::
animNameChanged(int ind)
{
  boneAnimName_ = animNameCombo_->itemText(ind);

  updateApp();
  updateMatrix();
}

void
CQNewGLBonesControl::
animTimeSlot(double t)
{
  boneAnimTime_ = t;

  updateApp();
  updateMatrix();
}

//---

CQNewGLTerrainControl::
CQNewGLTerrainControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  //--

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() << "Noise" << "Water Surface");
  addLabelEdit("Type", typeCombo_ );

  widthEdit_ = new CQRealSpin;
  addLabelEdit("Width", widthEdit_ );

  heightEdit_ = new CQRealSpin;
  addLabelEdit("Height", heightEdit_);

  gridSizeEdit_ = new CQIntegerSpin;
  addLabelEdit("Grid Size", gridSizeEdit_);

  textureCheck_ = new QCheckBox;
  addLabelEdit("Textured", textureCheck_);

  gridCheck_ = new QCheckBox;
  addLabelEdit("Show Grid", gridCheck_);

  layout->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *generateButton = new QPushButton("Generate");

  connect(generateButton, &QPushButton::clicked, this, &CQNewGLTerrainControl::generateSlot);

  buttonsLayout->addWidget(generateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLTerrainControl::
connectSlots(bool b)
{
  if (b) {
    connect(typeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(typeSlot(int)));
    connect(widthEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLTerrainControl::widthSlot);
    connect(heightEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLTerrainControl::heightSlot);
    connect(gridSizeEdit_, SIGNAL(valueChanged(int)),
            this, SLOT(gridSizeSlot(int)));
    connect(textureCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(textureSlot(int)));
    connect(gridCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(gridSlot(int)));
  }
  else {
    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(typeSlot(int)));
    disconnect(widthEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLTerrainControl::widthSlot);
    disconnect(heightEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLTerrainControl::heightSlot);
    disconnect(gridSizeEdit_, SIGNAL(valueChanged(int)),
               this, SLOT(gridSizeSlot(int)));
    disconnect(textureCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(textureSlot(int)));
    disconnect(gridCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(gridSlot(int)));
  }
}

void
CQNewGLTerrainControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  widthEdit_ ->setValue(terrain->width ());
  heightEdit_->setValue(terrain->height());

  gridSizeEdit_->setValue(terrain->gridSize());

  textureCheck_->setChecked(terrain->isTextured());

  gridCheck_->setChecked(terrain->isWireframe());

  connectSlots(true);
}

void
CQNewGLTerrainControl::
typeSlot(int ind)
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  terrain->setType(ind == 0 ? CQNewGLTerrain::Type::NOISE : CQNewGLTerrain::Type::WATER_SURFACE);
}

void
CQNewGLTerrainControl::
widthSlot(double r)
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  terrain->setWidth(r);
}

void
CQNewGLTerrainControl::
heightSlot(double r)
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  terrain->setHeight(r);
}

void
CQNewGLTerrainControl::
gridSizeSlot(int i)
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  terrain->setGridSize(i);
}

void
CQNewGLTerrainControl::
textureSlot(int state)
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  terrain->setTextured(state);
  canvas->update();
}

void
CQNewGLTerrainControl::
gridSlot(int state)
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  terrain->setWireframe(state);
  canvas->update();
}

void
CQNewGLTerrainControl::
generateSlot()
{
  auto *canvas = this->canvas();

  canvas->updateTerrain();
  canvas->update();
}

//---

CQNewGLMazeControl::
CQNewGLMazeControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  widthEdit_ = new CQRealSpin;
  addLabelEdit("Width", widthEdit_ );

  heightEdit_ = new CQRealSpin;
  addLabelEdit("Height", heightEdit_);

  layout->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *generateButton = new QPushButton("Generate");

  connect(generateButton, &QPushButton::clicked, this, &CQNewGLMazeControl::generateSlot);

  buttonsLayout->addWidget(generateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLMazeControl::
connectSlots(bool b)
{
  if (b) {
    connect(widthEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLMazeControl::widthSlot);
    connect(heightEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLMazeControl::heightSlot);
  }
  else {
    disconnect(widthEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLMazeControl::widthSlot);
    disconnect(heightEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLMazeControl::heightSlot);
  }
}

void
CQNewGLMazeControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();
  auto *maze   = canvas->getMaze();

  widthEdit_ ->setValue(maze->width ());
  heightEdit_->setValue(maze->height());

  connectSlots(true);
}

void
CQNewGLMazeControl::
widthSlot(double r)
{
  auto *canvas = this->canvas();
  auto *maze   = canvas->getMaze();

  maze->setWidth(r);
}

void
CQNewGLMazeControl::
heightSlot(double r)
{
  auto *canvas = this->canvas();
  auto *maze   = canvas->getMaze();

  maze->setHeight(r);
}

void
CQNewGLMazeControl::
generateSlot()
{
  auto *canvas = this->canvas();

  canvas->updateMaze();
  canvas->update();
}

//---

CQNewGLSkyboxControl::
CQNewGLSkyboxControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  dirNameEdit_ = new QLineEdit;
  addLabelEdit("Directory", dirNameEdit_);

  widthEdit_ = new CQRealSpin;
  addLabelEdit("Width", widthEdit_);

  wireframeCheck_ = new QCheckBox;
  addLabelEdit("Wireframe", wireframeCheck_);

  layout->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *generateButton = new QPushButton("Generate");

  connect(generateButton, &QPushButton::clicked, this, &CQNewGLSkyboxControl::generateSlot);

  buttonsLayout->addWidget(generateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLSkyboxControl::
connectSlots(bool b)
{
  if (b) {
    connect(dirNameEdit_, SIGNAL(editingFinished()),
            this, SLOT(dirNameSlot()));
    connect(widthEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLSkyboxControl::widthSlot);
    connect(wireframeCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(wireframeSlot(int)));
  }
  else {
    disconnect(dirNameEdit_, SIGNAL(editingFinished()),
               this, SLOT(dirNameSlot()));
    disconnect(widthEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLSkyboxControl::widthSlot);
    disconnect(wireframeCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(wireframeSlot(int)));
  }
}

void
CQNewGLSkyboxControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();
  auto *skybox = canvas->getSkybox();

  dirNameEdit_   ->setText(skybox->dirName());
  widthEdit_     ->setValue(skybox->width());
  wireframeCheck_->setChecked(skybox->isWireframe());

  connectSlots(true);
}

void
CQNewGLSkyboxControl::
dirNameSlot()
{
  auto *canvas = this->canvas();
  auto *skybox = canvas->getSkybox();

  skybox->setDirName(dirNameEdit_->text());
}

void
CQNewGLSkyboxControl::
widthSlot(double w)
{
  auto *canvas = this->canvas();
  auto *skybox = canvas->getSkybox();

  skybox->setWidth(w);
}

void
CQNewGLSkyboxControl::
wireframeSlot(int state)
{
  auto *canvas = this->canvas();
  auto *skybox = canvas->getSkybox();

  skybox->setWireframe(state);
  canvas->update();
}

void
CQNewGLSkyboxControl::
generateSlot()
{
  auto *canvas = this->canvas();

  canvas->updateSkybox();
  canvas->update();
}

//---

CQNewGLEmitterControl::
CQNewGLEmitterControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

  emitterList_ = new QListWidget;

  emitterList_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(emitterList_);

  //--

  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  //---

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() << "Generator" << "Flocking" << "Fireworks");
  addLabelEdit("Type", typeCombo_ );

  enabledCheck_ = new QCheckBox;
  addLabelEdit("Enabled", enabledCheck_);

  runningCheck_ = new QCheckBox;
  addLabelEdit("Running", runningCheck_);

  positionEdit_ = new CQPoint3DEdit;
  positionEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Position", positionEdit_);

  minVelocityEdit_ = new CQPoint3DEdit;
  minVelocityEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Min Velocity", minVelocityEdit_);

  maxVelocityEdit_ = new CQPoint3DEdit;
  maxVelocityEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Max Velocity", maxVelocityEdit_);

  intervalSpin_ = new CQIntegerSpin;
  addLabelEdit("Interval", intervalSpin_);

  sizeSpin_ = new CQRealSpin;
  addLabelEdit("Size", sizeSpin_);

  gravitySpin_ = new CQRealSpin;
  addLabelEdit("Gravity", gravitySpin_);

  maxParticlesSpin_ = new CQIntegerSpin;
  addLabelEdit("Max Particles", maxParticlesSpin_);

  colorEdit_ = new CQColorEdit;
  addLabelEdit("Color", colorEdit_);

  wireframeCheck_ = new QCheckBox;
  addLabelEdit("Wireframe", wireframeCheck_);

  imageEdit_ = new QLineEdit;
  addLabelEdit("Image", imageEdit_);

  layout->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *addButton = new QPushButton("Add");

  connect(addButton, &QPushButton::clicked, this, &CQNewGLEmitterControl::addSlot);

  buttonsLayout->addWidget(addButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLEmitterControl::
connectSlots(bool b)
{
  if (b) {
    connect(emitterList_, &QListWidget::currentItemChanged,
            this, &CQNewGLEmitterControl::emitterSelectedSlot);

    connect(typeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(typeSlot(int)));
    connect(enabledCheck_, &QCheckBox::stateChanged,
            this, &CQNewGLEmitterControl::enabledSlot);
    connect(runningCheck_, &QCheckBox::stateChanged,
            this, &CQNewGLEmitterControl::runningSlot);
    connect(positionEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLEmitterControl::positionSlot);
    connect(minVelocityEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLEmitterControl::minVelocitySlot);
    connect(maxVelocityEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLEmitterControl::maxVelocitySlot);
    connect(intervalSpin_, SIGNAL(valueChanged(int)),
            this, SLOT(intervalSlot(int)));
    connect(sizeSpin_, SIGNAL(realValueChanged(double)),
            this, SLOT(sizeSlot(double)));
    connect(gravitySpin_, SIGNAL(realValueChanged(double)),
            this, SLOT(gravitySlot(double)));
    connect(maxParticlesSpin_, SIGNAL(valueChanged(int)),
            this, SLOT(maxParticlesSlot(int)));
    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(colorSlot(const QColor &)));
    connect(wireframeCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(wireframeSlot(int)));
    connect(imageEdit_, SIGNAL(editingFinished()),
            this, SLOT(imageSlot()));
  }
  else {
    disconnect(emitterList_, &QListWidget::currentItemChanged,
               this, &CQNewGLEmitterControl::emitterSelectedSlot);

    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(typeSlot(int)));
    disconnect(enabledCheck_, &QCheckBox::stateChanged,
               this, &CQNewGLEmitterControl::enabledSlot);
    disconnect(runningCheck_, &QCheckBox::stateChanged,
               this, &CQNewGLEmitterControl::runningSlot);
    disconnect(positionEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLEmitterControl::positionSlot);
    disconnect(minVelocityEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLEmitterControl::minVelocitySlot);
    disconnect(maxVelocityEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLEmitterControl::maxVelocitySlot);
    disconnect(intervalSpin_, SIGNAL(valueChanged(int)),
               this, SLOT(intervalSlot(int)));
    disconnect(sizeSpin_, SIGNAL(realValueChanged(double)),
               this, SLOT(sizeSlot(double)));
    disconnect(gravitySpin_, SIGNAL(realValueChanged(double)),
               this, SLOT(gravitySlot(double)));
    disconnect(maxParticlesSpin_, SIGNAL(valueChanged(int)),
               this, SLOT(maxParticlesSlot(int)));
    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(colorSlot(const QColor &)));
    disconnect(wireframeCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(wireframeSlot(int)));
    disconnect(imageEdit_, SIGNAL(editingFinished()),
               this, SLOT(imageSlot()));
  }
}

void
CQNewGLEmitterControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas  = this->canvas();

  //---

  if (listInvalid_) {
    listInvalid_ = false;

    //---

    emitterList_->clear();

    int ind = 0;

    for (auto *emitter : canvas->getEmitters()) {
      const auto &name = emitter->name();

      auto *item = new QListWidgetItem(name);

      emitterList_->addItem(item);

      item->setData(Qt::UserRole, ind);

      ++ind;
    }
  }

  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  enabledCheck_    ->setChecked(emitter->isEnabled());
  runningCheck_    ->setChecked(emitter->isRunning());
  positionEdit_    ->setValue(emitter->position());
  minVelocityEdit_ ->setValue(emitter->minVelocity().point());
  maxVelocityEdit_ ->setValue(emitter->maxVelocity().point());
  intervalSpin_    ->setValue(emitter->emitInterval());
  gravitySpin_     ->setValue(emitter->gravity());
  sizeSpin_        ->setValue(emitter->pointSize());
  maxParticlesSpin_->setValue(emitter->maxParticles());

  colorEdit_->setColor(RGBAToQColor(emitter->color()));

  wireframeCheck_->setChecked(emitter->isWireframe());

  imageEdit_->setText(emitter->imageName());

  connectSlots(true);
}

void
CQNewGLEmitterControl::
emitterSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = this->canvas();

  int ind = item->data(Qt::UserRole).toInt();

  canvas->setCurrentEmitterNum(ind);

  updateWidgets();

  canvas->update();
}

void
CQNewGLEmitterControl::
typeSlot(int ind)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  if      (ind == 0)
    emitter->setType(CQNewGLEmitter::Type::GENERATOR);
  else if (ind == 1)
    emitter->setType(CQNewGLEmitter::Type::FLOCKING);
  else if (ind == 2)
    emitter->setType(CQNewGLEmitter::Type::FIREWORKS);
}

void
CQNewGLEmitterControl::
enabledSlot(int state)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setEnabled(state);

  canvas->update();
}

void
CQNewGLEmitterControl::
runningSlot(int state)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setRunning(state);

  canvas->update();
}

void
CQNewGLEmitterControl::
positionSlot()
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  auto p = positionEdit_->getValue();

  emitter->setPosition(p);

  canvas->update();
}

void
CQNewGLEmitterControl::
minVelocitySlot()
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  auto p = minVelocityEdit_->getValue();

  emitter->setMinVelocity(CVector3D(p));

  canvas->update();
}

void
CQNewGLEmitterControl::
maxVelocitySlot()
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  auto p = maxVelocityEdit_->getValue();

  emitter->setMaxVelocity(CVector3D(p));

  canvas->update();
}

void
CQNewGLEmitterControl::
intervalSlot(int i)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setEmitInterval(i);

  canvas->update();
}

void
CQNewGLEmitterControl::
gravitySlot(double r)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setGravity(r);

  canvas->update();
}

void
CQNewGLEmitterControl::
sizeSlot(double r)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setPointSize(r);

  canvas->update();
}

void
CQNewGLEmitterControl::
maxParticlesSlot(int i)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setMaxParticles(i);

  canvas->update();
}

void
CQNewGLEmitterControl::
colorSlot(const QColor &c)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setColor(QColorToRGBA(c));
  canvas->update();
}

void
CQNewGLEmitterControl::
wireframeSlot(int state)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setWireframe(state);
  canvas->update();
}

void
CQNewGLEmitterControl::
imageSlot()
{
  auto *canvas = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setImageName(imageEdit_->text());
}

void
CQNewGLEmitterControl::
addSlot()
{
  auto *canvas = this->canvas();

  canvas->addEmitter();

  if (! canvas->isTimerRunning())
    canvas->setTimerRunning(true);

  listInvalid_ = true;

  updateWidgets();

  canvas->updateEmitters();
  canvas->update();
}

//---

CQNewGLFractalControl::
CQNewGLFractalControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  //---

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() << "Lorenz");
  addLabelEdit("Type", typeCombo_ );

  wireframeCheck_ = new QCheckBox;
  addLabelEdit("Wireframe", wireframeCheck_);

  layout->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *generateButton = new QPushButton("Generate");

  connect(generateButton, &QPushButton::clicked, this, &CQNewGLFractalControl::generateSlot);

  buttonsLayout->addWidget(generateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLFractalControl::
connectSlots(bool b)
{
  if (b) {
    connect(typeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(typeSlot(int)));
    connect(wireframeCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(wireframeSlot(int)));
  }
  else {
    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(typeSlot(int)));
    disconnect(wireframeCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(wireframeSlot(int)));
  }
}

void
CQNewGLFractalControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas  = this->canvas();
  auto *fractal = canvas->getFractal();

  wireframeCheck_->setChecked(fractal->isWireframe());

  connectSlots(true);
}

void
CQNewGLFractalControl::
typeSlot(int ind)
{
  auto *canvas  = this->canvas();
  auto *fractal = canvas->getFractal();

  if (ind == 0)
    fractal->setType(CQNewGLFractal::Type::LORENZ);
}

void
CQNewGLFractalControl::
wireframeSlot(int state)
{
  auto *canvas  = this->canvas();
  auto *fractal = canvas->getFractal();

  fractal->setWireframe(state);
  canvas->update();
}

void
CQNewGLFractalControl::
generateSlot()
{
  auto *canvas  = this->canvas();
  auto *fractal = canvas->getFractal();

  fractal->setActive(true);

  canvas->updateFractal();
  canvas->update();
}

//---

CQNewGLDrawTreeControl::
CQNewGLDrawTreeControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

#if 0
  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };
#endif

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *generateButton = new QPushButton("Generate");

  connect(generateButton, &QPushButton::clicked, this, &CQNewGLDrawTreeControl::generateSlot);

  buttonsLayout->addWidget(generateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLDrawTreeControl::
connectSlots(bool)
{
}

void
CQNewGLDrawTreeControl::
updateWidgets()
{
#if 0
  connectSlots(false);

  auto *canvas = this->canvas();
  auto *tree   = canvas->getDrawTree();

  connectSlots(true);
#endif
}

void
CQNewGLDrawTreeControl::
generateSlot()
{
  auto *canvas = this->canvas();
  auto *tree   = canvas->getDrawTree();

  tree->setActive(true);

  canvas->updateDrawTree();
  canvas->update();
}

//---

CQNewGLShapeControl::
CQNewGLShapeControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  //--

  shapesList_ = new QListWidget;

  shapesList_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(shapesList_);

  //---

  auto addLabelEdit = [&](const QString &label, QWidget *w, bool vertical=false) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
  };

  //---

  showCheck_ = new QCheckBox;
  addLabelEdit("Show", showCheck_);

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() <<
    "Box" << "Cone" << "Cube" << "Cylinder" << "Hyperboloid" << "Pyramid" << "Sphere" << "Torus");
  addLabelEdit("Type", typeCombo_);

  startEdit_ = new CQPoint3DEdit;
  startEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Start", startEdit_);

  endEdit_ = new CQPoint3DEdit;
  endEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("End", endEdit_);

  widthEdit_ = new CQRealSpin;
  addLabelEdit("Width", widthEdit_);

  colorEdit_ = new CQColorEdit;
  addLabelEdit("Color", colorEdit_);

  wireframeCheck_ = new QCheckBox;
  addLabelEdit("Wireframe", wireframeCheck_);

  solidCheck_ = new QCheckBox;
  addLabelEdit("Solid", solidCheck_);

  auto createTextureEdit = [&](const QString &label) {
    auto *edit = new CQNewGLTextureChooser(control_);

    addLabelEdit(label, edit);

    edit->updateWidgets();

    return edit;
  };

  textureEdit_ = createTextureEdit("Texture");

  layout->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *addButton    = new QPushButton("Add");
  auto *updateButton = new QPushButton("Update");

  connect(addButton, &QPushButton::clicked, this, &CQNewGLShapeControl::addSlot);
  connect(updateButton, &QPushButton::clicked, this, &CQNewGLShapeControl::updateSlot);

  buttonsLayout->addWidget(addButton);
  buttonsLayout->addWidget(updateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);

  updateWidgets();
}

void
CQNewGLShapeControl::
connectSlots(bool b)
{
  if (b) {
    connect(shapesList_, &QListWidget::currentItemChanged,
            this, &CQNewGLShapeControl::shapeSelectedSlot);

    connect(showCheck_, &QCheckBox::stateChanged,
            this, &CQNewGLShapeControl::showSlot);

    connect(typeCombo_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(typeSlot(int)));
    connect(startEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLShapeControl::startSlot);
    connect(endEdit_, &CQPoint3DEdit::editingFinished,
            this, &CQNewGLShapeControl::endSlot);
    connect(widthEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLShapeControl::widthSlot);
    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(colorSlot(const QColor &)));
    connect(wireframeCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(wireframeSlot(int)));
    connect(solidCheck_, SIGNAL(stateChanged(int)),
            this, SLOT(solidSlot(int)));

    connect(textureEdit_, SIGNAL(textureChanged()), this, SLOT(textureSlot()));
  }
  else {
    disconnect(shapesList_, &QListWidget::currentItemChanged,
               this, &CQNewGLShapeControl::shapeSelectedSlot);

    disconnect(showCheck_, &QCheckBox::stateChanged,
               this, &CQNewGLShapeControl::showSlot);

    disconnect(typeCombo_, SIGNAL(currentIndexChanged(int)),
               this, SLOT(typeSlot(int)));
    disconnect(startEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLShapeControl::startSlot);
    disconnect(endEdit_, &CQPoint3DEdit::editingFinished,
               this, &CQNewGLShapeControl::endSlot);
    disconnect(widthEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLShapeControl::widthSlot);
    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(colorSlot(const QColor &)));
    disconnect(wireframeCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(wireframeSlot(int)));
    disconnect(solidCheck_, SIGNAL(stateChanged(int)),
               this, SLOT(solidSlot(int)));

    disconnect(textureEdit_, SIGNAL(textureChanged()), this, SLOT(textureSlot()));
  }
}

void
CQNewGLShapeControl::
shapeSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = this->canvas();

  int ind = item->data(Qt::UserRole).toInt();

  canvas->setCurrentShape(ind);

  updateWidgets();

  canvas->update();
}

void
CQNewGLShapeControl::
showSlot(int state)
{
  auto *canvas = this->canvas();
  auto *shape  = canvas->getCurrentShape();
  if (! shape) return;

  shape->setVisible(state);

  canvas->update();
}

void
CQNewGLShapeControl::
typeSlot(int ind)
{
  shapeData_.type = CQNewGLShape::Type::NONE;

  if      (ind == 0) shapeData_.type = CQNewGLShape::Type::BOX;
  else if (ind == 1) shapeData_.type = CQNewGLShape::Type::CONE;
  else if (ind == 2) shapeData_.type = CQNewGLShape::Type::CUBE;
  else if (ind == 3) shapeData_.type = CQNewGLShape::Type::CYLINDER;
  else if (ind == 4) shapeData_.type = CQNewGLShape::Type::HYPERBOLOID;
  else if (ind == 5) shapeData_.type = CQNewGLShape::Type::PYRAMID;
  else if (ind == 6) shapeData_.type = CQNewGLShape::Type::SPHERE;
  else if (ind == 7) shapeData_.type = CQNewGLShape::Type::TORUS;
}

void
CQNewGLShapeControl::
startSlot()
{
  shapeData_.start = startEdit_->getValue();
}

void
CQNewGLShapeControl::
endSlot()
{
  shapeData_.end = endEdit_->getValue();
}

void
CQNewGLShapeControl::
widthSlot(double r)
{
  shapeData_.width = r;
}

void
CQNewGLShapeControl::
colorSlot(const QColor &c)
{
  shapeData_.color = QColorToRGBA(c);
}

void
CQNewGLShapeControl::
wireframeSlot(int state)
{
  shapeData_.wireframe = state;
}

void
CQNewGLShapeControl::
solidSlot(int state)
{
  shapeData_.solid = state;
}

void
CQNewGLShapeControl::
textureSlot()
{
  shapeData_.textureName = textureEdit_->textureName();
}

void
CQNewGLShapeControl::
invalidateShapes()
{
  reload_ = true;

  updateWidgets();
}

void
CQNewGLShapeControl::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();

  //---

  if (reload_) {
    reload_ = false;

    shapesList_->clear();

    int ind = 0;

    for (auto *shape : canvas->shapes()) {
      const auto &name = shape->name();

      auto *item = new QListWidgetItem(name);

      shapesList_->addItem(item);

      item->setData(Qt::UserRole, ind++);
    }
  }

  //---

  bool visible = true;

  auto *shape = canvas->getCurrentShape();

  if (shape) {
    visible = shape->isVisible();

    shapeData_.type      = shape->type();
    shapeData_.start     = shape->start();
    shapeData_.end       = shape->end();
    shapeData_.width     = shape->width();
    shapeData_.color     = shape->color();
    shapeData_.wireframe = shape->isWireframe();
    shapeData_.solid     = shape->isSolid();

    if (shape->texture())
      shapeData_.textureName = QString::fromStdString(shape->texture()->getName());
    else
      shapeData_.textureName = "";
  }

  int ind = 0;

  if      (shapeData_.type == CQNewGLShape::Type::BOX        ) ind = 0;
  else if (shapeData_.type == CQNewGLShape::Type::CONE       ) ind = 1;
  else if (shapeData_.type == CQNewGLShape::Type::CUBE       ) ind = 2;
  else if (shapeData_.type == CQNewGLShape::Type::CYLINDER   ) ind = 3;
  else if (shapeData_.type == CQNewGLShape::Type::HYPERBOLOID) ind = 4;
  else if (shapeData_.type == CQNewGLShape::Type::PYRAMID    ) ind = 5;
  else if (shapeData_.type == CQNewGLShape::Type::SPHERE     ) ind = 6;
  else if (shapeData_.type == CQNewGLShape::Type::TORUS      ) ind = 7;

  typeCombo_     ->setCurrentIndex(ind);
  showCheck_     ->setChecked(visible);
  startEdit_     ->setValue(shapeData_.start);
  endEdit_       ->setValue(shapeData_.end);
  widthEdit_     ->setValue(shapeData_.width);
  colorEdit_     ->setColor(RGBAToQColor(shapeData_.color));
  wireframeCheck_->setChecked(shapeData_.wireframe);
  solidCheck_    ->setChecked(shapeData_.solid);
  textureEdit_   ->setTextureName(shapeData_.textureName);

  showCheck_->setEnabled(shape);

  //---

  connectSlots(true);
}

void
CQNewGLShapeControl::
addSlot()
{
  auto *canvas = this->canvas();

  auto *shape = canvas->addShape();

  shape->setType     (shapeData_.type);
  shape->setColor    (shapeData_.color);
  shape->setStart    (shapeData_.start);
  shape->setEnd      (shapeData_.end);
  shape->setWidth    (shapeData_.width);
  shape->setWireframe(shapeData_.wireframe);
  shape->setSolid    (shapeData_.solid);
  shape->setTexture  (canvas->getTextureByName(shapeData_.textureName.toStdString()));

  shape->invalidateGeometry();
  shape->addGeometry();

  canvas->update();

  invalidateShapes();
}

void
CQNewGLShapeControl::
updateSlot()
{
  auto *canvas = this->canvas();
  auto *shape  = canvas->getCurrentShape();
  if (! shape) return;

  shape->setType     (shapeData_.type);
  shape->setColor    (shapeData_.color);
  shape->setStart    (shapeData_.start);
  shape->setEnd      (shapeData_.end);
  shape->setWidth    (shapeData_.width);
  shape->setWireframe(shapeData_.wireframe);
  shape->setSolid    (shapeData_.solid);
  shape->setTexture  (canvas->getTextureByName(shapeData_.textureName.toStdString()));

  shape->invalidateGeometry();
  shape->addGeometry();

  canvas->update();
}

//---

CQNewGLObjectsList::
CQNewGLObjectsList(CQNewGLControl *control) :
 QFrame(nullptr), control_(control)
{
  auto *layout = new QVBoxLayout(this);

  tree_ = new QTreeWidget;

  tree_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(tree_);

  //---

  connectSlots(true);
}

void
CQNewGLObjectsList::
updateObjects()
{
  connectSlots(false);

  //---

  auto *canvas = control_->canvas();

  tree_->clear();

  objectItem_.clear();

  int ind = -1;

  auto *item = new QTreeWidgetItem(tree_, QStringList());

  tree_->addTopLevelItem(item);

  item->setData(0, Qt::UserRole, ind++);

  auto *scene = canvas->scene();

  for (auto *object : scene->getObjects()) {
    auto *item = createObjectItem(object);

    if (! item->parent())
      tree_->addTopLevelItem(item);
    else
      item->parent()->addChild(item);

    item->setData(0, Qt::UserRole, ind++);
  }

  //---

  connectSlots(true);
}

QTreeWidgetItem *
CQNewGLObjectsList::
createObjectItem(CGeomObject3D *object)
{
  auto pi = objectItem_.find(object);

  if (pi != objectItem_.end())
    return (*pi).second;

  auto name = QString::fromStdString(object->getName());

  auto *parent = object->parent();

  QTreeWidgetItem *item;

  if (parent) {
    auto *parentItem = createObjectItem(parent);

    item = new QTreeWidgetItem(parentItem, QStringList() << name);
  }
  else
    item = new QTreeWidgetItem(tree_, QStringList() << name);

  objectItem_[object] = item;

  return item;
}

void
CQNewGLObjectsList::
connectSlots(bool b)
{
  auto *canvas = control_->canvas();

  if (b) {
    connect(canvas, SIGNAL(modelAdded()), this, SLOT(updateObjects()));
    connect(tree_, &QTreeWidget::currentItemChanged,
            this, &CQNewGLObjectsList::objectSelectedSlot);
  }
  else {
    disconnect(canvas, SIGNAL(modelAdded()), this, SLOT(updateObjects()));
    disconnect(tree_, &QTreeWidget::currentItemChanged,
               this, &CQNewGLObjectsList::objectSelectedSlot);
  }
}

void
CQNewGLObjectsList::
objectSelectedSlot(QTreeWidgetItem *item, QTreeWidgetItem *)
{
  selectedInd_ = (item ? item->data(0, Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

CGeomObject3D *
CQNewGLObjectsList::
getSelectedIndObject() const
{
  auto *canvas = control_->canvas();

  return canvas->getObject(selectedInd_);
}

CGeomObject3D *
CQNewGLObjectsList::
getObjectListSelected() const
{
  auto *canvas = control_->canvas();

  auto selectedItems = tree_->selectedItems();

  int ind = -1;

  for (const auto *item : selectedItems) {
    int ind1 = item->data(0, Qt::UserRole).toInt();
    if (ind1 >= 0) {
      ind = ind1;
      break;
    }
  }

  return canvas->getObject(ind);
}
