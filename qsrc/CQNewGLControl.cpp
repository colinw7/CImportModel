#include <CQNewGLControl.h>
#include <CQNewGLModel.h>
#include <CQNewGLModelObject.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLUVMap.h>
#include <CQNewGLCamera.h>
#include <CQNewGLLight.h>
#include <CQNewGLAxes.h>
#include <CQNewGLBBox.h>
#include <CQNewGLNormals.h>
#include <CQNewGLWireframe.h>
#include <CQNewGLHull.h>
#include <CQNewGLBasis.h>
#include <CQNewGLTerrain.h>
#include <CQNewGLMaze.h>
#include <CQNewGLSkybox.h>
#include <CQNewGLEmitter.h>
#include <CQNewGLFractalWidget.h>
#include <CQNewGLFractal.h>
#include <CQNewGLDrawTree.h>
#include <CQNewGLShape.h>
#include <CQNewGLObjectsList.h>
#include <CQNewGLTextureChooser.h>
#include <CQNewGLAnimChooser.h>
#include <CQNewGLBonesList.h>
#include <CQNewGLShapesList.h>
#include <CQNewGLCamerasList.h>
#include <CQNewGLLightsList.h>
#include <CQNewGLUtil.h>

#include <CImportBase.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>
#include <CGeomNodeData.h>
#include <CQGLTexture.h>
#include <CQUtil.h>

#include <CQColorEdit.h>
#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQSlider.h>
#include <CQPoint3DEdit.h>
#include <CQMatrix3D.h>
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

#include <set>

#include <svg/play_svg.h>
#include <svg/pause_svg.h>
#include <svg/play_one_svg.h>

namespace {

QString cleanupLabel(const QString &label) {
  QString str;
  for (int i = 0; i < label.length(); ++i) {
    if (label[i].isSpace())
      str += "_";
    else
      str += label[i];
  }
  return str;
}

}

//---

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

  modelTab_ = CQUtil::makeWidget<QTabWidget>("modelTab");

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

  annotationsControl_ = new CQNewGLAnnotationsControl(this);

  addModelTab(annotationsControl_, "Annotations");

  axesControl_ = new CQNewGLAxesControl(this);

  addModelTab(axesControl_, "Axes");

  texturesControl_ = new CQNewGLTexturesControl(this);

  addModelTab(texturesControl_, "Textures");

  animControl_ = new CQNewGLAnimControl(this);

  addModelTab(animControl_, "Animation");

  bonesControl_ = new CQNewGLBonesControl(this);

  addModelTab(bonesControl_, "Bones");

  //---

  extrasTab_ = CQUtil::makeWidget<QTabWidget>("extrasTab");

  extrasTab_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  tab_->addTab(extrasTab_, "Extras");

  int extrasTabNum = 0;

  auto addExtrasTab = [&](QWidget *w, const QString &label) {
    extrasTabName_[extrasTabNum++] = label;

    extrasTab_->addTab(w, label);
  };

  //---

  terrainControl_ = new CQNewGLTerrainControl(this);

  addExtrasTab(terrainControl_, "Terrain");

  mazeControl_ = new CQNewGLMazeControl(this);

  addExtrasTab(mazeControl_, "Maze");

  skyboxControl_ = new CQNewGLSkyboxControl(this);

  addExtrasTab(skyboxControl_, "Skybox");

  emitterControl_ = new CQNewGLEmitterControl(this);

  addExtrasTab(emitterControl_, "Emitter");

  drawTreeControl_ = new CQNewGLDrawTreeControl(this);

  addExtrasTab(drawTreeControl_, "Draw Tree");

  shapeControl_ = new CQNewGLShapeControl(this);

  addExtrasTab(shapeControl_, "Shape");

  //---

  layout->addStretch(1);

  //---

  connect(modelTab_, SIGNAL(currentChanged(int)), this, SLOT(modelTabSlot(int)));

  connect(canvas(), SIGNAL(textureAdded()), this, SLOT(invalidateTextures()));

  connect(canvas(), SIGNAL(cameraAdded()), this, SLOT(invalidateCameras()));
  connect(canvas(), SIGNAL(lightAdded()), this, SLOT(invalidateLights()));
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

  app_->setShowBone(tabName == "Bones");

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

  updateAnnotations();

  updateAxes();

  updateTextures();
  updateAnim();
  updateBones();

  updateTerrain();
  updateMaze();

  updateSkybox();

  updateEmitter();

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
updateAnnotations()
{
  annotationsControl_->updateWidgets();
}

void
CQNewGLControl::
updateAxes()
{
  axesControl_->updateWidgets();
}

void
CQNewGLControl::
updateTextures()
{
  texturesControl_->updateWidgets();
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

#if 0
CGeomObject3D *
CQNewGLControl::
getRootObject() const
{
  auto *object = canvas()->getObject(0);

  while (object && object->parent())
    object = object->parent();

  return object;
}
#endif

//---

CQNewGLGeneralControl::
CQNewGLGeneralControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("generalControl");

  layout_ = new QVBoxLayout(this);

  //---

  colorEdit_ = addLabelEdit("Bg Color", new CQColorEdit(this));

  //---

  startGroup("GL Options");

  depthTestCheck_  = addGroupLabelEdit("Depth Test" , new QCheckBox);
  cullCheck_       = addGroupLabelEdit("Cull Face"  , new QCheckBox);
  frontFaceCheck_  = addGroupLabelEdit("Front Face" , new QCheckBox);
  showOrientCheck_ = addGroupLabelEdit("Show Orient", new QCheckBox);

  frontFaceCheck_->setToolTip("Front Face (Clockwise=1, Counter Clockwise=0)");

  endGroup();

  //---

  startGroup("Control");

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

  addGroupWidget(modelCheck);
  addGroupWidget(lightCheck);
  addGroupWidget(cameraCheck);

  endGroup();

  //---

  layout_->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLGeneralControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, colorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(bgColorSlot(const QColor &)));

  CQUtil::connectDisconnect(b, depthTestCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(depthTestSlot(int)));
  CQUtil::connectDisconnect(b, cullCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(cullSlot(int)));
  CQUtil::connectDisconnect(b, frontFaceCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(frontFaceSlot(int)));
  CQUtil::connectDisconnect(b, showOrientCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showOrientSlot(int)));
}

void
CQNewGLGeneralControl::
updateWidgets()
{
  auto *canvas = this->canvas();

  //---

  connectSlots(false);

  //---

  colorEdit_->setColor(canvas->bgColor());

  depthTestCheck_ ->setChecked(canvas->isDepthTest());
  cullCheck_      ->setChecked(canvas->isCullFace());
  frontFaceCheck_ ->setChecked(canvas->isFrontFace());
  showOrientCheck_->setChecked(canvas->isOrientColor());

  //---

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

  CBBox3D bbox;
  canvas->updateModelData(bbox);

  canvas->update();
}

void
CQNewGLGeneralControl::
showOrientSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setOrientColor(state);

  CBBox3D bbox;
  canvas->updateModelData(bbox);

  canvas->update();
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
  setObjectName("cameralControl");

  layout_ = new QVBoxLayout(this);

  //---

  camerasList_ = new CQNewGLCamerasList(control);

  layout_->addWidget(camerasList_);

  //---

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QGridLayout(controlFrame);

  layout_->addWidget(controlFrame);

  int row = 0;

  auto addControlLabelEdit = [&](const QString &label, auto *w) {
    controlLayout->addWidget(new QLabel(label), row, 0);
    controlLayout->addWidget(w, row, 1);
    ++row;
    return w;
  };

  //---

  showCheck_    = addControlLabelEdit("Show"    , new QCheckBox);
  followCheck_  = addControlLabelEdit("Follow"  , new QCheckBox);
  eyelineCheck_ = addControlLabelEdit("Eye Line", new QCheckBox);

  //---

  orthoCheck_ = addControlLabelEdit("Ortho", new QCheckBox);

  //---

  updateCurrentCheck_ = addControlLabelEdit("Update Current", new QCheckBox);

  rotateCheck_   = addControlLabelEdit("Rotate"   , new QCheckBox);
  rotateAtCombo_ = addControlLabelEdit("Rotate At", new QComboBox);

  rotateAtCombo_->addItems(QStringList() << "Camera" << "Origin");

  strafeCheck_ = addControlLabelEdit("Strafe", new QCheckBox);

  //---

  zoomEdit_  = addControlLabelEdit("Zoom" , new CQRealSpin);
  nearEdit_  = addControlLabelEdit("Near" , new CQRealSpin);
  farEdit_   = addControlLabelEdit("Far"  , new CQRealSpin);
  yawEdit_   = addControlLabelEdit("Yaw"  , new CQRealSpin);
  pitchEdit_ = addControlLabelEdit("Pitch", new CQRealSpin);
  rollEdit_  = addControlLabelEdit("Roll" , new CQRealSpin);

  //---

  speedEdit_ = addControlLabelEdit("Speed", new CQRealSpin);

  //---

  originEdit_ = addControlLabelEdit("Origin", new CQPoint3DEdit);
  originEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  positionEdit_ = addControlLabelEdit("Position", new CQPoint3DEdit);
  positionEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  upEdit_ = addControlLabelEdit("Up", new CQPoint3DEdit);
  upEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  rightEdit_ = addControlLabelEdit("Right", new CQPoint3DEdit);
  rightEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  //---

  matrixEdit_ = new CQMatrix3D;
  matrixEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addControlLabelEdit("Matrix", matrixEdit_);

  //---

  colorEdit_ = new CQColorEdit;
  addControlLabelEdit("Color", colorEdit_);

  //---

  controlLayout->setRowStretch(row, 1);

  //--

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

  auto *addButton   = new QPushButton("Add");
  auto *resetButton = new QPushButton("Reset");
  auto *initButton  = new QPushButton("Init");

  connect(addButton  , SIGNAL(clicked()), this, SLOT(addSlot()));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));
  connect(initButton , SIGNAL(clicked()), this, SLOT(initSlot()));

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
  CQUtil::connectDisconnect(b, camerasList_, SIGNAL(currentItemChanged()),
                            this, SLOT(updateWidgets()));

  CQUtil::connectDisconnect(b, showCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showSlot(int)));

  CQUtil::connectDisconnect(b, followCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(followSlot(int)));
  CQUtil::connectDisconnect(b, eyelineCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(eyelineSlot(int)));

  CQUtil::connectDisconnect(b, orthoCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(orthoSlot(int)));

  CQUtil::connectDisconnect(b, updateCurrentCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(updateCurrentSlot(int)));

  CQUtil::connectDisconnect(b, rotateCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(rotateSlot(int)));
  CQUtil::connectDisconnect(b, rotateAtCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(rotateAtSlot(int)));

  CQUtil::connectDisconnect(b, strafeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(strafeSlot(int)));

  CQUtil::connectDisconnect(b, zoomEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zoomSlot(double)));
  CQUtil::connectDisconnect(b, nearEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(nearSlot(double)));
  CQUtil::connectDisconnect(b, farEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(farSlot(double)));
  CQUtil::connectDisconnect(b, yawEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(yawSlot(double)));
  CQUtil::connectDisconnect(b, pitchEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(pitchSlot(double)));
  CQUtil::connectDisconnect(b, rollEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(rollSlot(double)));

  CQUtil::connectDisconnect(b, speedEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(speedSlot(double)));

  CQUtil::connectDisconnect(b, originEdit_, SIGNAL(editingFinished()),
                            this, SLOT(originSlot()));
  CQUtil::connectDisconnect(b, positionEdit_, SIGNAL(editingFinished()),
                            this, SLOT(positionSlot()));
  CQUtil::connectDisconnect(b, upEdit_, SIGNAL(editingFinished()),
                            this, SLOT(upSlot()));
  CQUtil::connectDisconnect(b, rightEdit_, SIGNAL(editingFinished()),
                            this, SLOT(rightSlot()));

  CQUtil::connectDisconnect(b, colorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(colorSlot(const QColor &)));
}

CQNewGLCamera *
CQNewGLCameraControl::
camera() const
{
  if (camerasList_->isUpdateCurrent())
    return canvas()->getCurrentCamera();
  else
    return camerasList_->getCurrentCamera();
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
updateCurrentSlot(int b)
{
  camerasList_->setUpdateCurrent(b);
}

void
CQNewGLCameraControl::
zoomSlot(double r)
{
  camera()->setFov(r);
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
  if (! camera) return;

  //---

  connectSlots(false);

  //---

  if (camerasInvalid_) {
    camerasInvalid_ = false;

    camerasList_->updateWidgets();
  }

  //---

  showCheck_->setChecked(canvas->isShowCameras());

  followCheck_->setChecked(camera->isFollowObject());

  eyelineCheck_->setChecked(canvas->isShowEyeline());

  orthoCheck_->setChecked(canvas->isOrtho());

  updateCurrentCheck_->setChecked(camerasList_->isUpdateCurrent());

  rotateCheck_->setChecked(camera->isRotate());

  rotateAtCombo_->setCurrentIndex(camera->rotateAt() == CQNewGLCamera::RotateAt::POSITION ? 0 : 1);

  strafeCheck_->setChecked(camera->isStrafe());

  zoomEdit_->setValue(camera->fov());

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

CQNewGLAnnotationsControl::
CQNewGLAnnotationsControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("annotationsControl");

  layout_ = new QVBoxLayout(this);

  //---

  startGroup("Normals");

  showNormalsCheck_    = addGroupLabelEdit("Model Normals"  , new QCheckBox);
  showNewNormalsCheck_ = addGroupLabelEdit("Generic Normals", new QCheckBox);

  //---

  normalsSizeEdit_  = addGroupLabelEdit("Size" , new CQRealSpin);
  normalsColorEdit_ = addGroupLabelEdit("Color", new CQColorEdit(this));

  //---

  tangentSpaceCheck_ = addGroupLabelEdit("Tangent Space", new QCheckBox(this));

  endGroup();

  //---

  startGroup("BBox");

  showBBoxCheck_ = addGroupLabelEdit("Show", new QCheckBox);

  endGroup();

  //---

  startGroup("Hull");

  showHullCheck_ = addGroupLabelEdit("Show", new QCheckBox);

  endGroup();

  //---

  startGroup("Basis");

  showBasisCheck_ = addGroupLabelEdit("Show", new QCheckBox);

  basisSizeEdit_  = addGroupLabelEdit("Size" , new CQRealSpin);
  basisWidthEdit_ = addGroupLabelEdit("Width", new CQRealSpin);

  basisUEdit_ = addGroupLabelEdit("U (Right)"  , new CQPoint3DEdit);
  basisVEdit_ = addGroupLabelEdit("V (Up)"     , new CQPoint3DEdit);
  basisWEdit_ = addGroupLabelEdit("W (Forward)", new CQPoint3DEdit);

  endGroup();

  //---

  startGroup("Wireframe");

  showWireframeCheck_ = addGroupLabelEdit("Show", new QCheckBox);

  endGroup();

  //---

  layout_->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLAnnotationsControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, showNormalsCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showNormalsSlot(int)));
  CQUtil::connectDisconnect(b, showNewNormalsCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showNewNormalsSlot(int)));
  CQUtil::connectDisconnect(b, normalsSizeEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(normalsSizeSlot(double)));
  CQUtil::connectDisconnect(b, normalsColorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(normalsColorSlot(const QColor &)));
  CQUtil::connectDisconnect(b, tangentSpaceCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(tangentSpaceSlot(int)));

  CQUtil::connectDisconnect(b, showBBoxCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showBBoxSlot(int)));

  CQUtil::connectDisconnect(b, showHullCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showHullSlot(int)));

  CQUtil::connectDisconnect(b, showBasisCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showBasisSlot(int)));
  CQUtil::connectDisconnect(b, basisSizeEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(basisSizeSlot(double)));
  CQUtil::connectDisconnect(b, basisWidthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(basisWidthSlot(double)));

  CQUtil::connectDisconnect(b, basisUEdit_, SIGNAL(editingFinished()),
                            this, SLOT(basisUSlot()));
  CQUtil::connectDisconnect(b, basisVEdit_, SIGNAL(editingFinished()),
                            this, SLOT(basisVSlot()));
  CQUtil::connectDisconnect(b, basisWEdit_, SIGNAL(editingFinished()),
                            this, SLOT(basisWSlot()));

  CQUtil::connectDisconnect(b, showWireframeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showWireframeSlot(int)));

  connect(canvas(), SIGNAL(currentObjectChanged()), this, SLOT(updateWidgets()));
}

void
CQNewGLAnnotationsControl::
updateWidgets()
{
  auto *canvas = this->canvas();

  //---

  connectSlots(false);

  //---

  auto *normals = canvas->getNormals();

  showNormalsCheck_->setChecked(canvas->isShowNormals());
  showNewNormalsCheck_->setChecked(normals ? normals->isVisible() : false);

  normalsSizeEdit_ ->setValue(canvas->normalsSize());
  normalsColorEdit_->setColor(canvas->normalsColor());

  tangentSpaceCheck_->setChecked(canvas->isTangentSpaceNormal());

  //---

  auto *bbox = canvas->getBBox();

  showBBoxCheck_->setChecked(bbox ? bbox->isVisible() : false);

  //---

  auto *hull = canvas->getHull();

  showHullCheck_->setChecked(hull ? hull->isVisible() : false);

  //---

  auto *basis = canvas->getBasis();

  showBasisCheck_->setChecked(basis ? basis->isShow() : false);
  basisSizeEdit_ ->setValue(basis ? basis->lineSize() : 0.0);
  basisWidthEdit_->setValue(basis ? basis->lineWidth() : 0.0);

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

  //---

  auto *wireframe = canvas->getWireframe();

  showWireframeCheck_->setChecked(wireframe ? wireframe->isVisible() : false);

  //---

  connectSlots(true);
}

void
CQNewGLAnnotationsControl::
showNormalsSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setShowNormals(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
showNewNormalsSlot(int state)
{
  auto *canvas  = this->canvas();
  auto *normals = canvas->getNormals();
  if (! normals) return;

  normals->setVisible(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
normalsSizeSlot(double s)
{
  auto *canvas  = this->canvas();
  auto *normals = canvas->getNormals();
  if (! normals) return;

  canvas ->setNormalsSize(s);
  normals->setLineSize(s);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
normalsColorSlot(const QColor &c)
{
  auto *canvas  = this->canvas();
  auto *normals = canvas->getNormals();
  if (! normals) return;

  canvas ->setNormalsColor(c);
  normals->setLineColor(c);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
tangentSpaceSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setTangentSpaceNormal(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
showBBoxSlot(int state)
{
  auto *canvas = this->canvas();
  auto *bbox   = canvas->getBBox();

  bbox->setVisible(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
showHullSlot(int state)
{
  auto *canvas = this->canvas();
  auto *hull   = canvas->getHull();

  hull->setVisible(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
showBasisSlot(int state)
{
  auto *canvas = this->canvas();
  auto *basis  = canvas->getBasis();

  basis->setShow(state);
  canvas->update();
}

void
CQNewGLAnnotationsControl::
basisSizeSlot(double s)
{
  auto *canvas = this->canvas();
  auto *basis  = canvas->getBasis();

  basis->setLineSize(s);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
basisWidthSlot(double w)
{
  auto *canvas = this->canvas();
  auto *basis  = canvas->getBasis();

  basis->setLineWidth(w);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnnotationsControl::
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
CQNewGLAnnotationsControl::
basisVSlot()
{
  basisUSlot();
}

void
CQNewGLAnnotationsControl::
basisWSlot()
{
  basisUSlot();
}

void
CQNewGLAnnotationsControl::
showWireframeSlot(int state)
{
  auto *canvas    = this->canvas();
  auto *wireframe = canvas->getWireframe();

  wireframe->setVisible(state);

  canvas->updateObjectsData();
  canvas->update();
}

//---

CQNewGLLightsControl::
CQNewGLLightsControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("lightsControl");

  layout_ = new QVBoxLayout(this);

  //---

  auto *materialGroup  = new QGroupBox("Material");
  auto *materialLayout = new QVBoxLayout(materialGroup);

  layout_->addWidget(materialGroup);

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

  lightsList_ = new CQNewGLLightsList(control);

  layout_->addWidget(lightsList_);

  //---

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QGridLayout(controlFrame);

  layout_->addWidget(controlFrame);

  int row = 0;

  auto addControlLabelEdit = [&](const QString &label, QWidget *w) {
    controlLayout->addWidget(new QLabel(label), row, 0);
    controlLayout->addWidget(w, row, 1);
    ++row;
  };

  //---

  showCheck_ = new QCheckBox;
  addControlLabelEdit("Show", showCheck_);

  //---

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() << "Directional" << "Point" << "Spot");
  addControlLabelEdit("Type", typeCombo_);

  enabledCheck_ = new QCheckBox;
  addControlLabelEdit("Enabled", enabledCheck_);

  positionEdit_ = new CQPoint3DEdit;
  addControlLabelEdit("Position", positionEdit_);

  directionEdit_ = new CQPoint3DEdit;
  addControlLabelEdit("Direction", directionEdit_);

  colorEdit_ = new CQColorEdit;
  addControlLabelEdit("Color", colorEdit_);

  radiusEdit_ = new CQRealSpin;
  addControlLabelEdit("Radius", radiusEdit_);

  cutoffEdit_ = new CQRealSpin;
  addControlLabelEdit("Cut Off", cutoffEdit_);

  //---

  controlLayout->setRowStretch(row, 1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

  auto *addButton = new QPushButton("Add");

  connect(addButton, SIGNAL(clicked()), this, SLOT(addSlot()));

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
  CQUtil::connectDisconnect(b, lightsList_, SIGNAL(currentItemChanged()),
                            this, SLOT(updateWidgets()));

  CQUtil::connectDisconnect(b, ambientEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(ambientSlot(const QColor &)));
  CQUtil::connectDisconnect(b, diffuseEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(diffuseSlot(const QColor &)));
  CQUtil::connectDisconnect(b, emissionEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(emissionSlot(const QColor &)));

  CQUtil::connectDisconnect(b, showCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showSlot(int)));

  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));

  CQUtil::connectDisconnect(b, enabledCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(enabledSlot(int)));

  CQUtil::connectDisconnect(b, positionEdit_, SIGNAL(editingFinished()),
                            this, SLOT(positionSlot()));
  CQUtil::connectDisconnect(b, directionEdit_, SIGNAL(editingFinished()),
                            this, SLOT(directionSlot()));

  CQUtil::connectDisconnect(b, colorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(colorSlot(const QColor &)));

  if (b) {
    connect(radiusEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLLightsControl::radiusSlot);
    connect(cutoffEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLLightsControl::cutoffSlot);
  }
  else {
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
  auto *light  = canvas->getCurrentLight();
  if (! light) return;

  //---

  connectSlots(false);

  //---

  ambientEdit_ ->setColor(canvas->ambientColor());
  diffuseEdit_ ->setColor(canvas->diffuseColor());
  emissionEdit_->setColor(canvas->emissionColor());

  if (lightsInvalid_) {
    lightsInvalid_ = false;

    lightsList_->updateWidgets();
  }

  //---

  showCheck_->setChecked(canvas->isShowLights());

  //---

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
  setObjectName("axesControl");

  layout_ = new QVBoxLayout(this);

  //---

  axisCheck_ = new QCheckBox("Show Axis");

  layout_->addWidget(axisCheck_);

  layout_->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLAxesControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, axisCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showSlot(int)));
}

void
CQNewGLAxesControl::
updateWidgets()
{
  auto *canvas = this->canvas();
  auto *axes   = canvas->getAxes();
  if (! axes) return;

  //---

  connectSlots(false);

  //---

  axisCheck_->setChecked(axes->isVisible());

  //---

  connectSlots(true);
}

void
CQNewGLAxesControl::
showSlot(int state)
{
  auto *canvas = this->canvas();

  auto *axes = canvas->getAxes();

  axes->setVisible(state);
  canvas->update();
}

//---

#if 0
CQNewGLBBoxControl::
CQNewGLBBoxControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("bboxControl");

  layout_ = new QVBoxLayout(this);

  //---

  showCheck_ = new QCheckBox("Show BBox");

  layout_->addWidget(showCheck_);

  layout_->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLBBoxControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, showCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showSlot(int)));
}

void
CQNewGLBBoxControl::
updateWidgets()
{
  connectSlots(false);

  //---

  auto *canvas = this->canvas();
  auto *bbox   = canvas->getBBox();

  showCheck_->setChecked(bbox->isVisible());

  //---

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
#endif

//---

#if 0
CQNewGLHullControl::
CQNewGLHullControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("hullControl");

  layout_ = new QVBoxLayout(this);

  //---

  showCheck_ = new QCheckBox("Show Hull");

  layout_->addWidget(showCheck_);

  layout_->addStretch(1);

  //---

  connectSlots(true);
}

void
CQNewGLHullControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, showCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showSlot(int)));
}

void
CQNewGLHullControl::
updateWidgets()
{
  connectSlots(false);

  //---

  auto *canvas = this->canvas();
  auto *hull   = canvas->getHull();

  showCheck_->setChecked(hull->isVisible());

  //---

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
#endif

//---

CQNewGLObjectsControl::
CQNewGLObjectsControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("objectsControl");

  layout_ = new QVBoxLayout(this);

  auto *groupBox    = new QGroupBox("Objects");
  auto *groupLayout = new QVBoxLayout(groupBox);

  objectsList_ = new CQNewGLObjectsList(control->canvas());

  groupLayout->addWidget(objectsList_);

  layout_->addWidget(groupBox);

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

  auto createTextureEdit = [&](const QString &label) {
    auto *edit = new CQNewGLTextureChooser(control_);

    return addLabelEdit(label, edit);
  };

#if 0
  auto createStringEdit = [&](const QString &label) {
    return addLabelEdit(label, new QLineEdit);
  };
#endif

  //---

  solidCheck_     = addLabelEdit("Solid"    , new QCheckBox);
  wireframeCheck_ = addLabelEdit("Wireframe", new QCheckBox);

  //---

  flipYZCheck_      = addLabelEdit("Flip YZ", new QCheckBox);
  invertDepthCheck_ = addLabelEdit("Invert Depth", new QCheckBox);

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

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

  auto *addButton   = new QPushButton("Add");
  auto *resetButton = new QPushButton("Reset Transform");

  connect(addButton, SIGNAL(clicked()), this, SLOT(addSlot()));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));

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
  CQUtil::connectDisconnect(b, objectsList_, SIGNAL(currentItemChanged()),
                            this, SLOT(objectSelectedSlot()));

  CQUtil::connectDisconnect(b, selectButton_  , SIGNAL(clicked()), this, SLOT(selectSlot()));
  CQUtil::connectDisconnect(b, deselectButton_, SIGNAL(clicked()), this, SLOT(deselectSlot()));
  CQUtil::connectDisconnect(b, showButton_    , SIGNAL(clicked()), this, SLOT(showSlot()));
  CQUtil::connectDisconnect(b, hideButton_    , SIGNAL(clicked()), this, SLOT(hideSlot()));
  CQUtil::connectDisconnect(b, currentButton_ , SIGNAL(clicked()), this, SLOT(currentSlot()));

  CQUtil::connectDisconnect(b, solidCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(solidSlot(int)));
  CQUtil::connectDisconnect(b, wireframeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(wireframeSlot(int)));

  CQUtil::connectDisconnect(b, flipYZCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(flipYZSlot(int)));
  CQUtil::connectDisconnect(b, invertDepthCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(invertDepthSlot(int)));

  CQUtil::connectDisconnect(b, centerEdit_, SIGNAL(editingFinished()), this, SLOT(centerSlot()));
  CQUtil::connectDisconnect(b, sizeEdit_  , SIGNAL(editingFinished()), this, SLOT(sizeSlot()));

#if 0
  CQUtil::connectDisconnect(b, translateEdit_, SIGNAL(editingFinished()),
                            this, SLOT(translateSlot()));
  CQUtil::connectDisconnect(b, rotateEdit_, SIGNAL(editingFinished()),
                            this, SLOT(rotateSlot()));
#endif

  CQUtil::connectDisconnect(b, diffuseTextureEdit_ , SIGNAL(textureChanged()),
                            this, SLOT(diffuseMapSlot()));
  CQUtil::connectDisconnect(b, normalTextureEdit_  , SIGNAL(textureChanged()),
                            this, SLOT(normalMapSlot()));
  CQUtil::connectDisconnect(b, specularTextureEdit_, SIGNAL(textureChanged()),
                            this, SLOT(specularMapSlot()));
  CQUtil::connectDisconnect(b, emissiveTextureEdit_, SIGNAL(textureChanged()),
                            this, SLOT(emissiveMapSlot()));
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
  auto *app = control_->app();

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
    auto *objectData = canvas()->getObjectData(object);

    solidCheck_    ->setChecked(objectData->isSolid    ());
    wireframeCheck_->setChecked(objectData->isWireframe());

    auto bbox = app->getObjectBBox(object);

    centerEdit_->setValue(bbox.getCenter());
    sizeEdit_  ->setValue(bbox.getSize().point());

    flipYZCheck_     ->setChecked(objectData->isFlipYZ());
    invertDepthCheck_->setChecked(objectData->isInvertDepth());
  }
  else {
    solidCheck_    ->setChecked(false);
    wireframeCheck_->setChecked(false);

    centerEdit_->setValue(CPoint3D(0, 0, 0));
    sizeEdit_  ->setValue(CPoint3D(1, 1, 1));

    flipYZCheck_     ->setChecked(false);
    invertDepthCheck_->setChecked(false);
  }

  //----

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
}

void
CQNewGLObjectsControl::
selectSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  object->setSelected(true);

  canvas()->update();

  selectButton_  ->setEnabled(false);
  deselectButton_->setEnabled(true);
}

void
CQNewGLObjectsControl::
deselectSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  object->setSelected(false);

  canvas()->update();

  selectButton_  ->setEnabled(true);
  deselectButton_->setEnabled(false);
}

void
CQNewGLObjectsControl::
showSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  object->setVisible(true);

  canvas()->updateObjectsData();
  canvas()->update();

  showButton_->setEnabled(false);
  hideButton_->setEnabled(true);
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
solidSlot(int state)
{
  auto *canvas = this->canvas();
  auto *object = getObjectListSelected();

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  objectData->setHierSolid(state);

  canvas->update();
}

void
CQNewGLObjectsControl::
wireframeSlot(int state)
{
  auto *canvas = this->canvas();
  auto *object = getObjectListSelected();

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  objectData->setHierWireframe(state);

  canvas->update();
}

void
CQNewGLObjectsControl::
flipYZSlot(int state)
{
  auto *canvas = this->canvas();
  auto *object = getObjectListSelected();

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  objectData->setFlipYZ(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLObjectsControl::
invertDepthSlot(int state)
{
  auto *canvas = this->canvas();
  auto *object = getObjectListSelected();

  auto *objectData = canvas->getObjectData(object);
  if (! objectData) return;

  objectData->setInvertDepth(state);

  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLObjectsControl::
centerSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  auto *app = control_->app();

  auto bbox = app->getObjectBBox(object);

  auto c1 = bbox.getCenter();
  auto c2 = centerEdit_->getValue();

  auto t = CMatrix3D::translation(c2.x - c1.x, c2.y - c1.y, c2.z - c1.z);

  object->setTransform(t*object->getTransform());

  canvas()->updateObjectsData();
  canvas()->update();

  //---

  bbox = app->getObjectBBox(object);

  centerEdit_->setValue(bbox.getCenter());
  sizeEdit_  ->setValue(bbox.getSize().point());
}

void
CQNewGLObjectsControl::
sizeSlot()
{
  auto *object = getObjectListSelected();
  if (! object) return;

  auto *app = control_->app();

  auto bbox = app->getObjectBBox(object);

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

  bbox = app->getObjectBBox(object);

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
  auto *canvas = control_->canvas();

  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setDiffuseTexture(texture);

    canvas->updateObjectsData();
    canvas->update();

    uvMap()->update();
  }
}

void
CQNewGLObjectsControl::
normalMapSlot()
{
  auto *canvas = control_->canvas();

  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setNormalTexture(texture);

    canvas->updateObjectsData();
    canvas->update();
  }
}

void
CQNewGLObjectsControl::
specularMapSlot()
{
  auto *canvas = control_->canvas();

  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setSpecularTexture(texture);

    canvas->updateObjectsData();
    canvas->update();
  }
}

void
CQNewGLObjectsControl::
emissiveMapSlot()
{
  auto *canvas = control_->canvas();

  auto *edit = qobject_cast<CQNewGLTextureChooser *>(sender());

  auto textureName = edit->textureName();

  auto *texture = canvas->getGeomTextureByName(textureName.toStdString());

  auto *object = getObjectListSelected();

  if (object) {
    object->setEmissiveTexture(texture);

    canvas->updateObjectsData();
    canvas->update();
  }
}

void
CQNewGLObjectsControl::
addSlot()
{
  auto *canvas = control_->canvas();
  auto *app    = canvas->app();

//auto dir = QDir::current().dirName();
  auto dir = app->buildDir() + "/models";

  auto fileName = QFileDialog::getOpenFileName(this, "Open Model File", dir, "Files (*.*)");
  if (! fileName.length()) return;

  auto format = CImportBase::filenameToType(fileName.toStdString());

  CQNewGLModel::LoadData loadData;

  if (! app->loadModel(fileName, format, loadData))
    std::cerr << "Failed to load model '" << fileName.toStdString() << "'\n";

  CBBox3D bbox;
  canvas->updateModelData(bbox);

  canvas->update();
}

void
CQNewGLObjectsControl::
resetSlot()
{
  auto *object = getObjectListSelected();

  if (object) {
    object->setTransform(CMatrix3D::identity());

    canvas()->updateAnnotations();
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
  setObjectName("texturesControl");

  layout_ = new QVBoxLayout(this);

  //---

  texturesList_ = new QListWidget;

  texturesList_->setSelectionMode(QListWidget::SingleSelection);

  layout_->addWidget(texturesList_);

  //---

  image_ = new CQNewGLTextureImage(this);

  image_->setFixedSize(QSize(256, 256));

  layout_->addWidget(image_);

  //---

  layout_->addStretch(1);

  //---

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QHBoxLayout(controlFrame);

  layout_->addWidget(controlFrame);

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

  //---

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

  //---

  connectSlots(true);
}

void
CQNewGLTexturesControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, texturesList_,
    SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    this, SLOT(textureSelectedSlot(QListWidgetItem *, QListWidgetItem *)));

//CQUtil::connectDisconnect(b, flipCheck_, SIGNAL(stateChanged(int)),
//                          this, SLOT(flipSlot(int));
  CQUtil::connectDisconnect(b, loadButton_, SIGNAL(clicked()),
                            this, SLOT(loadSlot()));
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

    auto *texture1 = CGeometry3DInst->createTexture(image);

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

  auto *canvas = control_->canvas();

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

#if 0
CQNewGLTextureChooser::
CQNewGLTextureChooser(CQNewGLControl *control) :
 control_(control)
{
  connectSlots(true);

  auto *canvas = control_->canvas();

  connect(canvas, SIGNAL(textureAdded()), this, SLOT(needsUpdateSlot()));

  auto w = QFontMetrics(font()).horizontalAdvance("X")*32;
  setFixedWidth(w);
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

  //---

  connectSlots(false);

  //---

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

  //---

  connectSlots(true);
}

void
CQNewGLTextureChooser::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this, SIGNAL(currentIndexChanged(int)),
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
#endif

//---

CQNewGLUVControl::
CQNewGLUVControl(CQNewGLModel *app) :
 app_(app)
{
  setObjectName("uvControl");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  layout_ = new QVBoxLayout(this);

  //---

  objectsList_ = new CQNewGLObjectsList(app->canvas());

  layout_->addWidget(objectsList_);

  typeCombo_ = new QComboBox;

  typeCombo_->addItems(QStringList() <<
    "None" << "Diffuse" << "Normal" << "Specular" << "Emissive");

  typeCombo_->setCurrentIndex(1);

  layout_->addWidget(typeCombo_);

  layout_->addStretch(1);

  //---

  connectSlots(true);

  updateWidgets();
}

void
CQNewGLUVControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, objectsList_, SIGNAL(currentItemChanged()),
                            this, SLOT(objectSelectedSlot()));

  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeChanged(int)));
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
updateWidgets()
{
  updateObjects();
}

void
CQNewGLUVControl::
objectSelectedSlot()
{
  auto *object = objectsList_->getSelectedIndObject();

  auto *uvMap = app_->uvMap();

  uvMap->setObject(object);
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

  auto *uvMap = app_->uvMap();

  uvMap->setTextureType(textureType);
}

//---

CQNewGLAnimControl::
CQNewGLAnimControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("animControl");

  layout_ = new QVBoxLayout(this);

  auto *app = control_->app();

  //---

  enabledCheck_ = addLabelEdit("Enabled", new QCheckBox);

  nameCombo_ = addLabelEdit("Name", new CQNewGLAnimChooser(app));
  timeEdit_  = addLabelEdit("Time", new CQRealSpin);

//objectsList_ = new CQNewGLObjectsList(control->canvas());

//layout_->addWidget(objectsList_);

  //---

  startGroup("Bones");

  bonesCheck_ = addGroupLabelEdit("Show Joints", new QCheckBox);

  boneVerticesCheck_ = addGroupLabelEdit("Show Vertices", new QCheckBox);

  bonesList_ = addGroupWidget(new CQNewGLBonesList(control));

  auto *boneControlFrame  = new QFrame;
  auto *boneControlLayout = new QHBoxLayout(boneControlFrame);

  boneSelectButton_   = new QPushButton("Select");
  boneDeselectButton_ = new QPushButton("Deselect");

  boneControlLayout->addWidget(boneSelectButton_);
  boneControlLayout->addWidget(boneDeselectButton_);
  boneControlLayout->addStretch(1);

  connect(boneSelectButton_, SIGNAL(clicked()), this, SLOT(boneSelectSlot()));
  connect(boneDeselectButton_, SIGNAL(clicked()), this, SLOT(boneDeselectSlot()));

  addGroupWidget(boneControlFrame);

  bonesTransform_ = addGroupLabelEdit("Bones Transform", new QComboBox);

  bonesTransform_->addItems(QStringList() <<
    "Inverse Bind" << "Local" << "Global");

  showBonePointsCheck_ = addGroupLabelEdit("Show Points", new QCheckBox);

  showBonePointsCheck_->setChecked(canvas()->isShowBonePoints());

  endGroup();

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

  layout_->addWidget(buttonFrame);

  //---

  layout_->addStretch(1);

  //---

  connectSlots(true);

  //---

  connect(app, SIGNAL(timerStep()), this, SLOT(timerSlot()));
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

  //---

  auto *canvas = control_->canvas();
  auto *app    = canvas->app();

  enabledCheck_->setChecked(app->isAnimEnabled());

  bonesCheck_       ->setChecked(canvas->isBonesEnabled());
  boneVerticesCheck_->setChecked(canvas->isShowBoneVertices());

  bonesList_->updateWidgets();

  if      (canvas->bonesTransform() == CQNewGLCanvas::BonesTransform::INVERSE_BIND)
    bonesTransform_->setCurrentIndex(0);
  else if (canvas->bonesTransform() == CQNewGLCanvas::BonesTransform::LOCAL)
    bonesTransform_->setCurrentIndex(1);
  else if (canvas->bonesTransform() == CQNewGLCanvas::BonesTransform::GLOBAL)
    bonesTransform_->setCurrentIndex(2);

  //---

  nameCombo_->updateWidgets();

  timeEdit_->setRange(nameCombo_->tmin(), nameCombo_->tmax());

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
  CQUtil::connectDisconnect(b, enabledCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(enabledSlot(int)));

  CQUtil::connectDisconnect(b, bonesCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(bonesSlot(int)));
  CQUtil::connectDisconnect(b, boneVerticesCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(boneVerticesSlot(int)));
  CQUtil::connectDisconnect(b, bonesList_, SIGNAL(currentItemChanged()),
                            this, SLOT(boneListSlot()));
  CQUtil::connectDisconnect(b, showBonePointsCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showBonePointsSlot(int)));

  CQUtil::connectDisconnect(b, bonesTransform_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(bonesTransformSlot(int)));
  CQUtil::connectDisconnect(b, nameCombo_, SIGNAL(animChanged()),
                            this, SLOT(nameChanged()));

  CQUtil::connectDisconnect(b, timeEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(timeSlot(double)));

  if (b) {
//  connect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
//          this, &CQNewGLAnimControl::objectSelectedSlot);

  }
  else {
//  disconnect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
//             this, &CQNewGLAnimControl::objectSelectedSlot);

  }
}

void
CQNewGLAnimControl::
enabledSlot(int state)
{
  auto *canvas = control_->canvas();
  auto *app    = canvas->app();

  app->setAnimEnabled(state);

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
boneVerticesSlot(int state)
{
  auto *canvas = control_->canvas();

  canvas->setShowBoneVertices(state);
  canvas->updateObjectsData();
  canvas->update();
}

void
CQNewGLAnimControl::
boneListSlot()
{
  auto *node = bonesList_->getBoneNode(bonesList_->boneInd());

  if (node) {
    boneSelectButton_  ->setEnabled(! node->isSelected());
    boneDeselectButton_->setEnabled(node->isSelected());
  }
  else {
    boneSelectButton_  ->setEnabled(false);
    boneDeselectButton_->setEnabled(false);
  }
}

void
CQNewGLAnimControl::
boneSelectSlot()
{
  auto *node = bonesList_->getBoneNode(bonesList_->boneInd());
  if (! node) return;

  node->setSelected(true);

  auto *canvas = this->canvas();
  canvas->updateObjectsData();
  canvas->update();

  boneSelectButton_  ->setEnabled(false);
  boneDeselectButton_->setEnabled(true);
}

void
CQNewGLAnimControl::
boneDeselectSlot()
{
  auto *node = bonesList_->getBoneNode(bonesList_->boneInd());
  if (! node) return;

  node->setSelected(false);

  auto *canvas = this->canvas();
  canvas->updateObjectsData();
  canvas->update();

  boneSelectButton_  ->setEnabled(true);
  boneDeselectButton_->setEnabled(false);
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
nameChanged()
{
  auto name = nameCombo_->animName();

  auto *canvas = control_->canvas();
  auto *app    = canvas->app();

  app->setAnimName(name);

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
  auto *app    = canvas->app();

  app->setTime(t);

  if (canvas->isShowBonePoints())
    canvas->updateObjectsData();

  canvas->update();
}

void
CQNewGLAnimControl::
playSlot()
{
  if (! running_) {
    auto *app = control_->app();

    app->startTimer();
  }

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
  setObjectName("bonesControl");

  layout_ = new QVBoxLayout(this);

  //---

  startGroup("Bones");

  bonesList_ = addGroupWidget(new CQNewGLBonesList(control));

  endGroup();

  //---

  matrixCombo_ = addLabelEdit("Matrix Type", new QComboBox);

  matrixCombo_->addItems(QStringList() <<
    "Inverse Bind" << "Local" << "Global" << "Anim");

  //---

  animNameCombo_ = addLabelEdit("Anim Name", new QComboBox);
  animTimeEdit_  = addLabelEdit("Anim Time", new CQRealSpin);

  //---

  isJointCheck_ = addLabelEdit("Is Joint", new QCheckBox);

  //---

  inverseCheck_ = addLabelEdit("Invert", new QCheckBox);

  //---

  auto vdata = LayoutData().setVertical();

  matrixEdit_ = new CQMatrix3D;

  addLabelEdit("Matrix", matrixEdit_, vdata);

  //---

  connectSlots(true);
}

void
CQNewGLBonesControl::
updateWidgets()
{
  connectSlots(false);

  //---

  bonesList_->updateWidgets();

  //---

  std::set<QString> animNames;

  animNameCombo_->clear();

  auto rootObjects = canvas()->getRootObjects();

  for (auto *rootObject : rootObjects) {
    std::vector<std::string> animNames1;
    rootObject->getAnimationNames(animNames1);

    for (const auto &animName1 : animNames1)
      animNames.insert(QString::fromStdString(animName1));
  }

  animNameCombo_->addItem("");

  for (const auto &name : animNames)
    animNameCombo_->addItem(name);

  //---

  connectSlots(true);
}

#if 0
CGeomObject3D *
CQNewGLBonesControl::
getBonesObject() const
{
  return canvas()->getBonesObject();
}
#endif

void
CQNewGLBonesControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, bonesList_, SIGNAL(currentItemChanged()),
                            this, SLOT(bonesListSlot()));

  CQUtil::connectDisconnect(b, matrixCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(matrixTypeChanged(int)));
  CQUtil::connectDisconnect(b, animNameCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(animNameChanged(int)));

  if (b) {
    connect(animTimeEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLBonesControl::animTimeSlot);
  }
  else {
    disconnect(animTimeEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLBonesControl::animTimeSlot);
  }

//CQUtil::connectDisconnect(b, isJointCheck_, SIGNAL(stateChanged(int)),
//                          this, SLOT(isJointSlot(int)));

  CQUtil::connectDisconnect(b, inverseCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(inverseSlot(int)));
}

void
CQNewGLBonesControl::
bonesListSlot()
{
  updateApp();
  updateMatrix();
}

void
CQNewGLBonesControl::
updateMatrix()
{
  connectSlots(false);

  auto *node = bonesList_->getBoneNode(bonesList_->boneInd());

  isJointCheck_->setChecked(node ? node->isJoint() : false);

  auto matrix = getMatrix();

  matrixEdit_->setValue(matrix);

  connectSlots(true);
}

void
CQNewGLBonesControl::
updateApp()
{
  auto *app = control_->app();

  app->setBoneInd     (bonesList_->boneInd());
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
  auto *node = bonesList_->getBoneNode(bonesList_->boneInd());

  if (! node)
    return CMatrix3D::identity();

  CMatrix3D matrix;

  if      (matrixType_ == MatrixType::INVERSE_BIND)
    matrix = node->inverseBindMatrix();
  else if (matrixType_ == MatrixType::LOCAL)
    matrix = node->localTransform();
  else if (matrixType_ == MatrixType::GLOBAL)
    matrix = node->globalTransform();
  else if (matrixType_ == MatrixType::ANIM) {
    auto *object = const_cast<CGeomObject3D *>(node->object());

    object->updateNodesAnimationData(boneAnimName_.toStdString(), boneAnimTime_);

    matrix = node->animMatrix();
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
  setObjectName("terrainControl");

  layout_ = new QVBoxLayout(this);

  //--

  typeCombo_ = addLabelEdit("Type", new QComboBox);
  typeCombo_->addItems(QStringList() << "Noise" << "Water Surface");

  widthEdit_  = addLabelEdit("Width" , new CQRealSpin);
  heightEdit_ = addLabelEdit("Height", new CQRealSpin);

  gridSizeEdit_ = addLabelEdit("Grid Size", new CQIntegerSpin);

  octavesEdit_ = addLabelEdit("Octaves", new CQIntegerSpin);

  textureCheck_ = addLabelEdit("Textured", new QCheckBox);

  gridCheck_ = addLabelEdit("Show Grid", new QCheckBox);

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

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
  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));
  CQUtil::connectDisconnect(b, widthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(widthSlot(double)));
  CQUtil::connectDisconnect(b, heightEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(heightSlot(double)));
  CQUtil::connectDisconnect(b, gridSizeEdit_, SIGNAL(valueChanged(int)),
                            this, SLOT(gridSizeSlot(int)));
  CQUtil::connectDisconnect(b, octavesEdit_, SIGNAL(valueChanged(int)),
                            this, SLOT(octavesSlot(int)));
  CQUtil::connectDisconnect(b, textureCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(textureSlot(int)));
  CQUtil::connectDisconnect(b, gridCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(gridSlot(int)));
}

void
CQNewGLTerrainControl::
updateWidgets()
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();
  if (! terrain) return;

  //---

  connectSlots(false);

  //---

  widthEdit_ ->setValue(terrain->width ());
  heightEdit_->setValue(terrain->height());

  gridSizeEdit_->setValue(terrain->gridSize());

  octavesEdit_->setValue(terrain->octaves());

  textureCheck_->setChecked(terrain->isTextured());

  gridCheck_->setChecked(terrain->isWireframe());

  //---

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
octavesSlot(int i)
{
  auto *canvas  = this->canvas();
  auto *terrain = canvas->getTerrain();

  terrain->setOctaves(i);
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
  setObjectName("mazeControl");

  layout_ = new QVBoxLayout(this);

  //--

  widthEdit_  = addLabelEdit("Width" , new CQRealSpin );
  heightEdit_ = addLabelEdit("Height", new CQRealSpin);

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

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
  CQUtil::connectDisconnect(b, widthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(widthSlot(double)));
  CQUtil::connectDisconnect(b, heightEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(heightSlot(double)));
}

void
CQNewGLMazeControl::
updateWidgets()
{
  auto *canvas = this->canvas();
  auto *maze   = canvas->getMaze();
  if (! maze) return;

  //---

  connectSlots(false);

  //---

  widthEdit_ ->setValue(maze->width ());
  heightEdit_->setValue(maze->height());

  //---

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
  auto *maze   = canvas->getMaze();

  maze->setVisible(true);

  canvas->updateObjectsData();
  canvas->update();
}

//---

CQNewGLSkyboxControl::
CQNewGLSkyboxControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("skyboxControl");

  layout_ = new QVBoxLayout(this);

  //--

  dirNameEdit_ = new QLineEdit;
  addLabelEdit("Directory", dirNameEdit_);

  widthEdit_ = new CQRealSpin;
  addLabelEdit("Width", widthEdit_);

  wireframeCheck_ = new QCheckBox;
  addLabelEdit("Wireframe", wireframeCheck_);

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

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
  CQUtil::connectDisconnect(b, dirNameEdit_, SIGNAL(editingFinished()),
                            this, SLOT(dirNameSlot()));

  if (b) {
    connect(widthEdit_, &CQRealSpin::realValueChanged,
            this, &CQNewGLSkyboxControl::widthSlot);
  }
  else {
    disconnect(widthEdit_, &CQRealSpin::realValueChanged,
               this, &CQNewGLSkyboxControl::widthSlot);
  }

  CQUtil::connectDisconnect(b, wireframeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(wireframeSlot(int)));
}

void
CQNewGLSkyboxControl::
updateWidgets()
{
  auto *canvas = this->canvas();
  auto *skybox = canvas->getSkybox();
  if (! skybox) return;

  //---

  connectSlots(false);

  //---

  dirNameEdit_   ->setText(skybox->dirName());
  widthEdit_     ->setValue(skybox->width());
  wireframeCheck_->setChecked(skybox->isWireframe());

  //---

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
  setObjectName("emitterControl");

  layout_ = new QVBoxLayout(this);

  //--

  emitterList_ = new QListWidget;

  emitterList_->setSelectionMode(QListWidget::SingleSelection);

  layout_->addWidget(emitterList_);

  //--

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() << "Generator" << "Flocking" << "Fireworks");
  addLabelEdit("Type", typeCombo_ );

  enabledCheck_ = addLabelEdit("Enabled", new QCheckBox);
  runningCheck_ = addLabelEdit("Running", new QCheckBox);

  positionEdit_ = new CQPoint3DEdit;
  positionEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Position", positionEdit_);

  minVelocityEdit_ = new CQPoint3DEdit;
  minVelocityEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Min Velocity", minVelocityEdit_);

  maxVelocityEdit_ = new CQPoint3DEdit;
  maxVelocityEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Max Velocity", maxVelocityEdit_);

  intervalSpin_ = addLabelEdit("Interval", new CQIntegerSpin);

  sizeSpin_ = addLabelEdit("Size", new CQRealSpin);

  gravitySpin_ = addLabelEdit("Gravity", new CQRealSpin);

  maxParticlesSpin_ = addLabelEdit("Max Particles", new CQIntegerSpin);

  maxAgeSpin_ = addLabelEdit("Max Age", new CQIntegerSpin);

  startColorEdit_ = addLabelEdit("Start Color", new CQColorEdit);
  endColorEdit_   = addLabelEdit("End Color", new CQColorEdit);

  wireframeCheck_ = addLabelEdit("Wireframe", new QCheckBox);

  imageEdit_ = addLabelEdit("Image", new QLineEdit);

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

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
  CQUtil::connectDisconnect(b, emitterList_,
    SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    this, SLOT(emitterSelectedSlot(QListWidgetItem *, QListWidgetItem *)));

  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));

  CQUtil::connectDisconnect(b, enabledCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(enabledSlot(int)));
  CQUtil::connectDisconnect(b, runningCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(runningSlot(int)));

  CQUtil::connectDisconnect(b, positionEdit_, SIGNAL(editingFinished()),
                            this, SLOT(positionSlot()));
  CQUtil::connectDisconnect(b, minVelocityEdit_, SIGNAL(editingFinished()),
                            this, SLOT(minVelocitySlot()));
  CQUtil::connectDisconnect(b, maxVelocityEdit_, SIGNAL(editingFinished()),
                            this, SLOT(maxVelocitySlot()));

  CQUtil::connectDisconnect(b, intervalSpin_, SIGNAL(valueChanged(int)),
                            this, SLOT(intervalSlot(int)));
  CQUtil::connectDisconnect(b, sizeSpin_, SIGNAL(realValueChanged(double)),
                            this, SLOT(sizeSlot(double)));
  CQUtil::connectDisconnect(b, gravitySpin_, SIGNAL(realValueChanged(double)),
                            this, SLOT(gravitySlot(double)));
  CQUtil::connectDisconnect(b, maxParticlesSpin_, SIGNAL(valueChanged(int)),
                            this, SLOT(maxParticlesSlot(int)));
  CQUtil::connectDisconnect(b, maxAgeSpin_, SIGNAL(valueChanged(int)),
                            this, SLOT(maxAgeSlot(int)));
  CQUtil::connectDisconnect(b, startColorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(startColorSlot(const QColor &)));
  CQUtil::connectDisconnect(b, endColorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(endColorSlot(const QColor &)));
  CQUtil::connectDisconnect(b, wireframeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(wireframeSlot(int)));
  CQUtil::connectDisconnect(b, imageEdit_, SIGNAL(editingFinished()),
                            this, SLOT(imageSlot()));
}

void
CQNewGLEmitterControl::
updateWidgets()
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  //---

  connectSlots(false);

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

  enabledCheck_    ->setChecked(emitter->isEnabled());
  runningCheck_    ->setChecked(emitter->isRunning());
  positionEdit_    ->setValue(emitter->position());
  minVelocityEdit_ ->setValue(emitter->minVelocity().point());
  maxVelocityEdit_ ->setValue(emitter->maxVelocity().point());
  intervalSpin_    ->setValue(emitter->emitInterval());
  gravitySpin_     ->setValue(emitter->gravity());
  sizeSpin_        ->setValue(emitter->pointSize());
  maxParticlesSpin_->setValue(emitter->maxParticles());
  maxAgeSpin_      ->setValue(emitter->maxAge());

  startColorEdit_->setColor(RGBAToQColor(emitter->startColor()));
  endColorEdit_  ->setColor(RGBAToQColor(emitter->endColor()));

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
maxAgeSlot(int i)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setMaxAge(i);

  canvas->update();
}

void
CQNewGLEmitterControl::
startColorSlot(const QColor &c)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setStartColor(QColorToRGBA(c));
  canvas->update();
}

void
CQNewGLEmitterControl::
endColorSlot(const QColor &c)
{
  auto *canvas  = this->canvas();
  auto *emitter = canvas->getCurrentEmitter();
  if (! emitter) return;

  emitter->setEndColor(QColorToRGBA(c));
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
  auto *app    = canvas->app();

  canvas->addEmitter();

  if (! app->isTimerRunning())
    app->setTimerRunning(true);

  listInvalid_ = true;

  updateWidgets();

  canvas->updateEmitters();
  canvas->update();
}

//---

CQNewGLFractalControl::
CQNewGLFractalControl(CQNewGLModel *app) :
 app_(app)
{
  setObjectName("fractalControl");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  layout_ = new QVBoxLayout(this);

  //--

  typeCombo_ = addLabelEdit("Type", new QComboBox);
  typeCombo_->addItems(QStringList() << "Lorenz");

  wireframeCheck_ = addLabelEdit("Wireframe" , new QCheckBox);
  textureCheck_   = addLabelEdit("Texture"   , new QCheckBox);
  axesCheck_      = addLabelEdit("Axes"      , new QCheckBox);
  pointSizeEdit_  = addLabelEdit("Point Size", new CQRealSpin);

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

  auto *generateButton = new QPushButton("Generate");

  connect(generateButton, SIGNAL(clicked()), this, SLOT(generateSlot()));

  buttonsLayout->addWidget(generateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);

  updateWidgets();
}

void
CQNewGLFractalControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));
  CQUtil::connectDisconnect(b, wireframeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(wireframeSlot(int)));
  CQUtil::connectDisconnect(b, textureCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(textureSlot(int)));
  CQUtil::connectDisconnect(b, axesCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(axesSlot(int)));
  CQUtil::connectDisconnect(b, pointSizeEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(pointSizeSlot(double)));
}

void
CQNewGLFractalControl::
updateWidgets()
{
  auto *fractalWidget = app_->fractalWidget();
  auto *fractal       = fractalWidget->fractal();
  if (! fractal) return;

  auto *axes = fractalWidget->getAxes();

  //---

  connectSlots(false);

  //---

  wireframeCheck_->setChecked(fractal->isWireframe());
  textureCheck_  ->setChecked(fractal->isTextured());
  axesCheck_     ->setChecked(axes ? axes->isVisible() : false);
  pointSizeEdit_ ->setValue  (fractal->pointSize());

  //---

  connectSlots(true);
}

void
CQNewGLFractalControl::
typeSlot(int ind)
{
  auto *fractalWidget = app_->fractalWidget();
  auto *fractal       = fractalWidget->fractal();

  if (ind == 0)
    fractal->setType(CQNewGLFractal::Type::LORENZ);
}

void
CQNewGLFractalControl::
wireframeSlot(int state)
{
  auto *fractalWidget = app_->fractalWidget();
  auto *fractal       = fractalWidget->fractal();

  fractal->setWireframe(state);

  fractalWidget->update();
}

void
CQNewGLFractalControl::
textureSlot(int state)
{
  auto *fractalWidget = app_->fractalWidget();
  auto *fractal       = fractalWidget->fractal();

  fractal->setTextured(state);

  fractalWidget->update();
}

void
CQNewGLFractalControl::
axesSlot(int state)
{
  auto *fractalWidget = app_->fractalWidget();

  auto *axes = fractalWidget->getAxes();
  if (! axes) return;

  axes->setVisible(state);

  fractalWidget->update();
}

void
CQNewGLFractalControl::
pointSizeSlot(double r)
{
  auto *fractalWidget = app_->fractalWidget();
  auto *fractal       = fractalWidget->fractal();

  fractal->setPointSize(r);

  fractalWidget->update();
}

void
CQNewGLFractalControl::
generateSlot()
{
  auto *fractalWidget = app_->fractalWidget();
  auto *fractal       = fractalWidget->fractal();

  fractal->setActive(true);

  fractalWidget->updateFractal();
  fractalWidget->update();
}

//---

CQNewGLDrawTreeControl::
CQNewGLDrawTreeControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("drawTreeControl");

  layout_ = new QVBoxLayout(this);

  //---

  widthEdit_  = addLabelEdit("Width" , new CQRealSpin);
  heightEdit_ = addLabelEdit("Height", new CQRealSpin);

  leftDirectionEdit_ = addLabelEdit("Left Direction", new CQPoint3DEdit);
  leftDirectionEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  rightDirectionEdit_ = addLabelEdit("Right Direction", new CQPoint3DEdit);
  rightDirectionEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  leftAlphaEdit_  = addLabelEdit("Left Alpha" , new CQRealSpin);
  rightAlphaEdit_ = addLabelEdit("Right Alpha", new CQRealSpin);

  depthEdit_ = addLabelEdit("Depth", new CQIntegerSpin);

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

  auto *generateButton = new QPushButton("Generate");

  connect(generateButton, &QPushButton::clicked, this, &CQNewGLDrawTreeControl::generateSlot);

  buttonsLayout->addWidget(generateButton);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);

  updateWidgets();
}

void
CQNewGLDrawTreeControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, widthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(widthSlot(double)));
  CQUtil::connectDisconnect(b, heightEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(heightSlot(double)));
  CQUtil::connectDisconnect(b, leftDirectionEdit_, SIGNAL(editingFinished()),
                            this, SLOT(leftDirectionSlot()));
  CQUtil::connectDisconnect(b, rightDirectionEdit_, SIGNAL(editingFinished()),
                            this, SLOT(rightDirectionSlot()));
  CQUtil::connectDisconnect(b, leftAlphaEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(leftAlphaSlot(double)));
  CQUtil::connectDisconnect(b, rightAlphaEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(rightAlphaSlot(double)));
  CQUtil::connectDisconnect(b, depthEdit_, SIGNAL(valueChanged(int)),
                            this, SLOT(depthSlot(int)));
}

void
CQNewGLDrawTreeControl::
updateWidgets()
{
  connectSlots(false);

  widthEdit_ ->setValue(width_);
  heightEdit_->setValue(height_);

  leftDirectionEdit_ ->setValue(
    CPoint3D(leftDirection_.getX(), leftDirection_.getY(), leftDirection_.getZ()));
  rightDirectionEdit_->setValue(
    CPoint3D(rightDirection_.getX(), rightDirection_.getY(), rightDirection_.getZ()));

  leftAlphaEdit_ ->setValue(leftAlpha_);
  rightAlphaEdit_->setValue(rightAlpha_);

  depthEdit_->setValue(depth_);

  connectSlots(true);
}

void
CQNewGLDrawTreeControl::
widthSlot(double w)
{
  width_ = w;
}

void
CQNewGLDrawTreeControl::
heightSlot(double h)
{
  height_ = h;
}

void
CQNewGLDrawTreeControl::
leftDirectionSlot()
{
  auto p = leftDirectionEdit_->getValue();

  leftDirection_  = CVector3D(p.x, p.y, p.z);
}

void
CQNewGLDrawTreeControl::
rightDirectionSlot()
{
  auto p = rightDirectionEdit_->getValue();

  rightDirection_ = CVector3D(p.x, p.y, p.z);
}

void
CQNewGLDrawTreeControl::
leftAlphaSlot(double w)
{
  leftAlpha_ = w;
}

void
CQNewGLDrawTreeControl::
rightAlphaSlot(double h)
{
  rightAlpha_ = h;
}

void
CQNewGLDrawTreeControl::
depthSlot(int depth)
{
  depth_ = depth;
}

void
CQNewGLDrawTreeControl::
generateSlot()
{
  auto *canvas = this->canvas();
  auto *tree   = canvas->getDrawTree();

  tree->setActive(true);

  tree->setWidth (width_);
  tree->setHeight(height_);

  tree->setLeftDirection (leftDirection_);
  tree->setRightDirection(rightDirection_);

  tree->setLeftAlpha (leftAlpha_);
  tree->setRightAlpha(rightAlpha_);

  tree->setDepth(depth_);

  canvas->updateDrawTree();
  canvas->update();
}

//---

CQNewGLShapeControl::
CQNewGLShapeControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  setObjectName("shapeControl");

  layout_ = new QVBoxLayout(this);

  //--

  shapesList_ = new CQNewGLShapesList(control);

  layout_->addWidget(shapesList_);

  //---

  auto labelWidth = QFontMetrics(font()).horizontalAdvance("Angle Delta") + 24;

  auto addLabelEdit = [&](const QString &label, auto *w, bool stretch=false) {
    auto name = cleanupLabel(label);

    w->setObjectName(name);

    auto *frame = new QFrame;
    frame->setObjectName(QString("frame_%1").arg(name));

    auto *layout1 = new QHBoxLayout(frame);

    auto *labelW = CQUtil::makeLabelWidget<QLabel>(label, "label");
    labelW->setFixedWidth(labelWidth);

    layout1->addWidget(labelW);
    layout1->addWidget(w);

    if (stretch)
      layout1->addStretch(1);

    layout_->addWidget(frame);

    return w;
  };

  //---

  typeCombo_ = new QComboBox;
  typeCombo_->addItems(QStringList() <<
    "Cone" << "Cube" << "Cylinder" << "Hyperboloid" << "Pyramid" << "Sphere" << "Torus");
  addLabelEdit("Type", typeCombo_);

  showCheck_      = addLabelEdit("Show"     , new QCheckBox);
  wireframeCheck_ = addLabelEdit("Wireframe", new QCheckBox);
  solidCheck_     = addLabelEdit("Solid"    , new QCheckBox);

  startEdit_ = addLabelEdit("Start", new CQPoint3DEdit, /*stretch*/true);
  startEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  endEdit_ = addLabelEdit("End", new CQPoint3DEdit, /*stretch*/true);
  endEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  widthEdit_ = addLabelEdit("Width", new CQRealSpin);
  colorEdit_ = addLabelEdit("Color", new CQColorEdit);

  angleStartEdit_ = addLabelEdit("Angle Start", new CQRealSpin);
  angleDeltaEdit_ = addLabelEdit("Angle Delta", new CQRealSpin);

  sizeEdit_ = new CQPoint3DEdit;
  sizeEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addLabelEdit("Size", sizeEdit_, /*stretch*/true);

  textureEdit_ = addLabelEdit("Texture", new CQNewGLTextureChooser(control_), /*stretch*/true);

  layout_->addStretch(1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout_->addWidget(buttonsFrame);

  addButton_    = new QPushButton("Add");
  updateButton_ = new QPushButton("Update");
  deleteButton_ = new QPushButton("Delete");

  connect(addButton_   , &QPushButton::clicked, this, &CQNewGLShapeControl::addSlot);
  connect(updateButton_, &QPushButton::clicked, this, &CQNewGLShapeControl::updateSlot);
  connect(deleteButton_, &QPushButton::clicked, this, &CQNewGLShapeControl::deleteSlot);

  buttonsLayout->addWidget(addButton_);
  buttonsLayout->addWidget(updateButton_);
  buttonsLayout->addWidget(deleteButton_);
  buttonsLayout->addStretch(1);

  //---

  connectSlots(true);

  updateWidgets();
}

void
CQNewGLShapeControl::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));

  CQUtil::connectDisconnect(b, showCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(showSlot(int)));
  CQUtil::connectDisconnect(b, wireframeCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(wireframeSlot(int)));
  CQUtil::connectDisconnect(b, solidCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(solidSlot(int)));

  CQUtil::connectDisconnect(b, startEdit_, SIGNAL(editingFinished()),
                            this, SLOT(startSlot()));
  CQUtil::connectDisconnect(b, endEdit_, SIGNAL(editingFinished()),
                            this, SLOT(endSlot()));
  CQUtil::connectDisconnect(b, widthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(widthSlot(double)));
  CQUtil::connectDisconnect(b, colorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(colorSlot(const QColor &)));
  CQUtil::connectDisconnect(b, angleStartEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(angleStartSlot(double)));
  CQUtil::connectDisconnect(b, angleDeltaEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(angleDeltaSlot(double)));
  CQUtil::connectDisconnect(b, sizeEdit_, SIGNAL(editingFinished()),
                            this, SLOT(sizeSlot()));
  CQUtil::connectDisconnect(b, textureEdit_, SIGNAL(textureChanged()),
                            this, SLOT(textureSlot()));
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
typeSlot(int ind)
{
  shapeData_.type = CQNewGLShape::Type::NONE;

  if      (ind == 0) shapeData_.type = CQNewGLShape::Type::CONE;
  else if (ind == 1) shapeData_.type = CQNewGLShape::Type::CUBE;
  else if (ind == 2) shapeData_.type = CQNewGLShape::Type::CYLINDER;
  else if (ind == 3) shapeData_.type = CQNewGLShape::Type::HYPERBOLOID;
  else if (ind == 4) shapeData_.type = CQNewGLShape::Type::PYRAMID;
  else if (ind == 5) shapeData_.type = CQNewGLShape::Type::SPHERE;
  else if (ind == 6) shapeData_.type = CQNewGLShape::Type::TORUS;

  updateState();
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
angleStartSlot(double r)
{
  shapeData_.angleStart = r;
}

void
CQNewGLShapeControl::
angleDeltaSlot(double r)
{
  shapeData_.angleDelta = r;
}

void
CQNewGLShapeControl::
sizeSlot()
{
  shapeData_.size = sizeEdit_->getValue();
}

void
CQNewGLShapeControl::
textureSlot()
{
  shapeData_.textureName = textureEdit_->textureName();
}

void
CQNewGLShapeControl::
updateWidgets()
{
  connectSlots(false);

  //---

  auto *canvas = this->canvas();

  //---

  shapesList_->updateWidgets();

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

  if      (shapeData_.type == CQNewGLShape::Type::CONE       ) ind = 0;
  else if (shapeData_.type == CQNewGLShape::Type::CUBE       ) ind = 1;
  else if (shapeData_.type == CQNewGLShape::Type::CYLINDER   ) ind = 2;
  else if (shapeData_.type == CQNewGLShape::Type::HYPERBOLOID) ind = 3;
  else if (shapeData_.type == CQNewGLShape::Type::PYRAMID    ) ind = 4;
  else if (shapeData_.type == CQNewGLShape::Type::SPHERE     ) ind = 5;
  else if (shapeData_.type == CQNewGLShape::Type::TORUS      ) ind = 6;

  typeCombo_     ->setCurrentIndex(ind);
  showCheck_     ->setChecked(visible);
  wireframeCheck_->setChecked(shapeData_.wireframe);
  solidCheck_    ->setChecked(shapeData_.solid);
  startEdit_     ->setValue(shapeData_.start);
  endEdit_       ->setValue(shapeData_.end);
  widthEdit_     ->setValue(shapeData_.width);
  colorEdit_     ->setColor(RGBAToQColor(shapeData_.color));
  angleStartEdit_->setValue(shapeData_.angleStart);
  angleDeltaEdit_->setValue(shapeData_.angleDelta);
  sizeEdit_      ->setValue(shapeData_.size);
  textureEdit_   ->setTextureName(shapeData_.textureName);

  showCheck_->setEnabled(shape);

  //---

  updateState();

  //---

  connectSlots(true);
}

void
CQNewGLShapeControl::
updateState()
{
  auto ind = typeCombo_->currentIndex();

  widthEdit_     ->setEnabled(ind == 0 || ind == 2 || ind == 4);
  angleStartEdit_->setEnabled(ind == 5);
  angleDeltaEdit_->setEnabled(ind == 5);
}

void
CQNewGLShapeControl::
addSlot()
{
  auto *canvas = this->canvas();
  auto *shape  = canvas->addShape();

  updateShape(shape);
}

void
CQNewGLShapeControl::
updateSlot()
{
  auto *canvas = this->canvas();
  auto *shape  = canvas->getCurrentShape();
  if (! shape) return;

  updateShape(shape);

  canvas->updateAnnotationObjects();
}

void
CQNewGLShapeControl::
deleteSlot()
{
  auto *canvas = this->canvas();
  auto *shape  = canvas->getCurrentShape();
  if (! shape) return;

  canvas->deleteShape(shape);

  canvas->updateObjectsData();
  canvas->updateAnnotationObjects();
  canvas->update();
}

void
CQNewGLShapeControl::
updateShape(CQNewGLShape *shape)
{
  auto *canvas = this->canvas();

  shape->setType      (shapeData_.type);
  shape->setColor     (shapeData_.color);
  shape->setStart     (shapeData_.start);
  shape->setEnd       (shapeData_.end);
  shape->setWidth     (shapeData_.width);
  shape->setWireframe (shapeData_.wireframe);
  shape->setAngleStart(shapeData_.angleStart);
  shape->setAngleDelta(shapeData_.angleDelta);
  shape->setSolid     (shapeData_.solid);
  shape->setSize      (shapeData_.size);
  shape->setTexture   (canvas->getTextureByName(shapeData_.textureName.toStdString()));

  shape->updateGeometry();
  shape->addGeometry();

  canvas->updateObjectsData();
  canvas->updateAnnotationObjects();
  canvas->update();
}
