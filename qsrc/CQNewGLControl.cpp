#include <CQNewGLControl.h>
#include <CQNewGLModel.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLUVMap.h>

#include <CImportBase.h>
#include <CGeometry3D.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>
#include <CGLCamera.h>
#include <CGLTexture.h>

#include <CQColorEdit.h>
#include <CQRealSpin.h>
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
#include <QTimer>

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

  generalControl_ = new CQNewGLGeneralControl(this);

  tab_->addTab(generalControl_, "General");

  objectsControl_ = new CQNewGLObjectsControl(this);

  tab_->addTab(objectsControl_, "Model");

  cameraControl_ = new CQNewGLCameraControl(this);

  tab_->addTab(cameraControl_, "Camera");

  lightControl_ = new CQNewGLLightControl(this);

  tab_->addTab(lightControl_, "Lights");

  texturesControl_ = new CQNewGLTexturesControl(this);

  tab_->addTab(texturesControl_, "Textures");

  uvControl_ = new CQNewGLUVControl(this);

  tab_->addTab(uvControl_, "UV");

  animControl_ = new CQNewGLAnimControl(this);

  tab_->addTab(animControl_, "Animation");

  bonesControl_ = new CQNewGLBonesControl(this);

  tab_->addTab(bonesControl_, "Bones");

  //---

  layout->addStretch(1);

  //---

  connect(tab_, SIGNAL(currentChanged(int)), this, SLOT(tabSlot(int)));
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
tabSlot(int ind)
{
  bool isShowBone = app_->isShowBone();

  app_->setShowUVMap(ind == 5);
  app_->setShowBone (ind == 7);

  if (app_->isShowBone() != isShowBone) {
    canvas()->updateObjectData();
    canvas()->update();
  }
}

void
CQNewGLControl::
updateWidgets()
{
  updateCamera();
  updateLights();
  updateObjects();
  updateTextures();
  updateUV();
  updateAnim();
  updateBones();
}

void
CQNewGLControl::
updateCamera()
{
  cameraControl_->updateWidgets();
}

void
CQNewGLControl::
updateLights()
{
  lightControl_->updateWidgets();
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

CGeomObject3D *
CQNewGLControl::
getRootObject() const
{
  const auto &scene = canvas()->importBase()->getScene();

  const auto &objects = scene.getObjects();
  if (objects.empty()) return nullptr;

  auto *object = objects[0];

  while (object->parent())
    object = object->parent();

  return object;
}

//---

CQNewGLGeneralControl::
CQNewGLGeneralControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *app    = control->app();
  auto *canvas = this->canvas();

  //---

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

  colorEdit_ = new CQColorEdit(this),

  colorEdit_->setColor(canvas->bgColor());

  addLabelEdit("Bg Color", colorEdit_);

  //---

  depthTestCheck_ = new QCheckBox("Depth Test");

  depthTestCheck_->setChecked(canvas->isDepthTest());

  layout->addWidget(depthTestCheck_);

  //---

  cullCheck_ = new QCheckBox("Cull Face");

  cullCheck_->setChecked(canvas->isCullFace());

  layout->addWidget(cullCheck_);

  //---

  frontFaceCheck_ = new QCheckBox("Front Face");

  frontFaceCheck_->setChecked(canvas->isFrontFace());

  layout->addWidget(frontFaceCheck_);

  //---

  polygonLineCheck_ = new QCheckBox("Polygon Line");

  polygonLineCheck_->setChecked(canvas->isPolygonLine());

  layout->addWidget(polygonLineCheck_);

  //---

  showNormalsCheck_ = new QCheckBox("Show Normals");

  showNormalsCheck_->setChecked(canvas->isShowNormals());

  layout->addWidget(showNormalsCheck_);

  //---

  flipYZCheck_ = new QCheckBox("Flip YZ");

  flipYZCheck_->setChecked(canvas->isFlipYZ());

  layout->addWidget(flipYZCheck_);

  //---

  axisCheck_ = new QCheckBox("Show Axis");

  axisCheck_->setChecked(canvas->isShowAxis());

  layout->addWidget(axisCheck_);

  //---

  orthoCheck_ = new QCheckBox("Ortho");

  orthoCheck_->setChecked(canvas->isOrtho());

  layout->addWidget(orthoCheck_);

  //---

  invertDepthCheck_ = new QCheckBox("Invert Depth");

  invertDepthCheck_->setChecked(canvas->isInvertDepth());

  layout->addWidget(invertDepthCheck_);

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

  ambientEdit_ = new CQColorEdit(this),

  ambientEdit_->setColor(canvas->ambientColor());

  addMaterialLabelEdit("Ambient", ambientEdit_);

  //---

  diffuseEdit_ = new CQColorEdit(this),

  diffuseEdit_->setColor(canvas->diffuseColor());

  addMaterialLabelEdit("Diffuse", diffuseEdit_);

  //---

  emissionEdit_ = new CQColorEdit(this),

  emissionEdit_->setColor(canvas->emissionColor());

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
  if (b) {
    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(bgColorSlot(const QColor &)));

    connect(ambientEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(ambientSlot(const QColor &)));
    connect(diffuseEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(diffuseSlot(const QColor &)));
    connect(emissionEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(emissionSlot(const QColor &)));

    connect(depthTestCheck_, SIGNAL(stateChanged(int)), this, SLOT(depthTestSlot(int)));

    connect(cullCheck_, SIGNAL(stateChanged(int)), this, SLOT(cullSlot(int)));

    connect(frontFaceCheck_, SIGNAL(stateChanged(int)), this, SLOT(frontFaceSlot(int)));

    connect(polygonLineCheck_, SIGNAL(stateChanged(int)), this, SLOT(polygonLineSlot(int)));

    connect(showNormalsCheck_, SIGNAL(stateChanged(int)), this, SLOT(showNormalsSlot(int)));

    connect(flipYZCheck_, SIGNAL(stateChanged(int)), this, SLOT(flipYZSlot(int)));

    connect(axisCheck_, SIGNAL(stateChanged(int)), this, SLOT(axisSlot(int)));

    connect(invertDepthCheck_, SIGNAL(stateChanged(int)), this, SLOT(invertDepthSlot(int)));

    connect(orthoCheck_, SIGNAL(stateChanged(int)), this, SLOT(orthoSlot(int)));
  }
  else {
    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(bgColorSlot(const QColor &)));

    disconnect(ambientEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(ambientSlot(const QColor &)));
    disconnect(diffuseEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(diffuseSlot(const QColor &)));
    disconnect(emissionEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(emissionSlot(const QColor &)));

    disconnect(depthTestCheck_, SIGNAL(stateChanged(int)), this, SLOT(depthTestSlot(int)));

    disconnect(cullCheck_, SIGNAL(stateChanged(int)), this, SLOT(cullSlot(int)));

    disconnect(frontFaceCheck_, SIGNAL(stateChanged(int)), this, SLOT(frontFaceSlot(int)));

    disconnect(polygonLineCheck_, SIGNAL(stateChanged(int)), this, SLOT(polygonLineSlot(int)));

    disconnect(showNormalsCheck_, SIGNAL(stateChanged(int)), this, SLOT(showNormalsSlot(int)));

    disconnect(flipYZCheck_, SIGNAL(stateChanged(int)), this, SLOT(flipYZSlot(int)));

    disconnect(axisCheck_, SIGNAL(stateChanged(int)), this, SLOT(axisSlot(int)));

    disconnect(invertDepthCheck_, SIGNAL(stateChanged(int)), this, SLOT(invertDepthSlot(int)));

    disconnect(orthoCheck_, SIGNAL(stateChanged(int)), this, SLOT(orthoSlot(int)));
  }
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
ambientSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setAmbientColor(c);

  canvas->updateObjectData();
  canvas->update();
}

void
CQNewGLGeneralControl::
diffuseSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setDiffuseColor(c);

  canvas->updateObjectData();
  canvas->update();
}

void
CQNewGLGeneralControl::
emissionSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setEmissionColor(c);

  canvas->updateObjectData();
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
polygonLineSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setPolygonLine(state);
  canvas->update();
}

void
CQNewGLGeneralControl::
showNormalsSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setShowNormals(state);

  canvas->updateObjectData();
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
axisSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setShowAxis(state);
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
orthoSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setOrtho(state);
}

void
CQNewGLGeneralControl::
uvMapSlot(int state)
{
  auto *app = control_->app();

  app->setShowUVMap(state);
}

void
CQNewGLGeneralControl::
ambientFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setAmbientStrength(v);
  canvas->update();
}

void
CQNewGLGeneralControl::
diffuseFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setDiffuseStrength(v);
  canvas->update();
}

void
CQNewGLGeneralControl::
specularFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setSpecularStrength(v);
  canvas->update();
}

void
CQNewGLGeneralControl::
emissiveFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setEmissiveStrength(v);
  canvas->update();
}

void
CQNewGLGeneralControl::
shininessFactorSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app->setShininess(v);

  canvas->updateObjectData();
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
  auto *layout = new QGridLayout(this);

  int row = 0;

  auto addLabelEdit = [&](const QString &label, QWidget *w) {
    layout->addWidget(new QLabel(label), row, 0);
    layout->addWidget(w, row, 1);
    ++row;
  };

  //---

  rotateCheck_ = new QCheckBox;
  addLabelEdit("Rotate", rotateCheck_);

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

  posEdit_ = new CQPoint3DEdit;
  addLabelEdit("Position", posEdit_);

  //--

  resetButton_ = new QPushButton("Reset");

  layout->addWidget(resetButton_);

  //---

  layout->setRowStretch(row, 1);

  connectSlots(true);
}

void
CQNewGLCameraControl::
connectSlots(bool b)
{
  if (b) {
    connect(rotateCheck_, &QCheckBox::stateChanged, this, &CQNewGLCameraControl::rotateSlot);
    connect(zoomEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::zoomSlot);
    connect(nearEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::nearSlot);
    connect(farEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::farSlot);
    connect(yawEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::yawSlot);
    connect(pitchEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::pitchSlot);
    connect(rollEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::rollSlot);
    connect(posEdit_, &CQPoint3DEdit::editingFinished, this, &CQNewGLCameraControl::posSlot);
    connect(resetButton_, &QPushButton::clicked, this, &CQNewGLCameraControl::resetSlot);
  }
  else {
    disconnect(rotateCheck_, &QCheckBox::stateChanged, this, &CQNewGLCameraControl::rotateSlot);
    disconnect(zoomEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::zoomSlot);
    disconnect(nearEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::nearSlot);
    disconnect(farEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::farSlot);
    disconnect(yawEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::yawSlot);
    disconnect(pitchEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::pitchSlot);
    disconnect(rollEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::rollSlot);
    disconnect(posEdit_, &CQPoint3DEdit::editingFinished, this, &CQNewGLCameraControl::posSlot);
    disconnect(resetButton_, &QPushButton::clicked, this, &CQNewGLCameraControl::resetSlot);
  }
}

CQNewGLCamera *
CQNewGLCameraControl::
camera() const
{
  return canvas()->camera();
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
  camera()->setRotate(b);
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
posSlot()
{
  auto p = posEdit_->getValue();

  camera()->setPosition(CGLVector3D(p.x, p.y, p.z));
  canvas()->update();
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
updateWidgets()
{
  connectSlots(false);

  auto *camera = this->camera();

  //---

  rotateCheck_->setChecked(camera->isRotate());

  zoomEdit_->setValue(camera->zoom());

  nearEdit_->setValue(camera->near());
  farEdit_ ->setValue(camera->far());

  yawEdit_  ->setValue(camera->yaw());
  pitchEdit_->setValue(camera->pitch());
  rollEdit_ ->setValue(camera->roll());

  auto p = camera->position();
  posEdit_->setValue(CPoint3D(p.x(), p.y(), p.z()));

  connectSlots(true);
}

//---

CQNewGLLightControl::
CQNewGLLightControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

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

  posEdit_ = new CQPoint3DEdit;
  addLabelEdit("Position", posEdit_);

  //---

  colorEdit_ = new CQColorEdit;
  addLabelEdit("Color", colorEdit_);

  //---

  controlLayout->setRowStretch(row, 1);

  //---

  auto *buttonsFrame  = new QFrame;
  auto *buttonsLayout = new QHBoxLayout(buttonsFrame);

  layout->addWidget(buttonsFrame);

  auto *addButton = new QPushButton("Add");

  connect(addButton, &QPushButton::clicked, this, &CQNewGLLightControl::addSlot);

  buttonsLayout->addWidget(addButton);
  buttonsLayout->addStretch(1);
}

void
CQNewGLLightControl::
connectSlots(bool b)
{
  if (b) {
    connect(lightsList_, &QListWidget::currentItemChanged,
            this, &CQNewGLLightControl::lightSelectedSlot);

    connect(posEdit_, &CQPoint3DEdit::editingFinished, this, &CQNewGLLightControl::posSlot);

    connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
            this, SLOT(colorSlot(const QColor &)));
  }
  else {
    disconnect(lightsList_, &QListWidget::currentItemChanged,
               this, &CQNewGLLightControl::lightSelectedSlot);

    disconnect(posEdit_, &CQPoint3DEdit::editingFinished, this, &CQNewGLLightControl::posSlot);

    disconnect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
               this, SLOT(colorSlot(const QColor &)));
  }
}

void
CQNewGLLightControl::
lightSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  auto *canvas = this->canvas();

  int ind = item->data(Qt::UserRole).toInt();

  canvas->setCurrentLight(ind);

  updateWidgets(/*reload*/false);

  canvas->update();
}

void
CQNewGLLightControl::
posSlot()
{
  auto *canvas = this->canvas();

  auto p = posEdit_->getValue();

  canvas->setCurrentLightPos(CGLVector3D(p.x, p.y, p.z));
  canvas->update();
}

void
CQNewGLLightControl::
colorSlot(const QColor &c)
{
  auto *canvas = this->canvas();

  canvas->setCurrentLightColor(c);
  canvas->update();
}

void
CQNewGLLightControl::
addSlot()
{
  auto *canvas = this->canvas();

  canvas->addLight();

  control_->updateLights();

  canvas->update();
}

void
CQNewGLLightControl::
updateWidgets(bool reload)
{
  auto *canvas = this->canvas();

  connectSlots(false);

  //---

  if (reload) {
    lightsList_->clear();

    int ind = 0;

    for (auto *light : canvas->lights()) {
      auto name = light->name;

      auto *item = new QListWidgetItem(name);

      lightsList_->addItem(item);

      item->setData(Qt::UserRole, ind++);
    }
  }

  //---

  auto p = canvas->currentLightPos();
  posEdit_->setValue(CPoint3D(p.x(), p.y(), p.z()));

  colorEdit_->setColor(canvas->currentLightColor());

  connectSlots(true);
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

  selectButton_   = new QPushButton("Select");
  deselectButton_ = new QPushButton("Deselect");
  showButton_     = new QPushButton("Show");
  hideButton_     = new QPushButton("Hide");

  showButton_->setEnabled(false);
  hideButton_->setEnabled(false);

  controlLayout->addWidget(selectButton_);
  controlLayout->addWidget(deselectButton_);
  controlLayout->addWidget(showButton_);
  controlLayout->addWidget(hideButton_);
  controlLayout->addStretch(1);

  groupLayout->addWidget(controlFrame);

  //---

  auto createPointEdit = [&](const QString &label) {
    auto *layout1 = new QHBoxLayout;

    layout->addLayout(layout1);

    auto *labelW = new QLabel(label);
    auto *edit   = new CQPoint3DEdit;

    layout1->addWidget(labelW);
    layout1->addWidget(edit);

    return edit;
  };

  auto createTextureEdit = [&](const QString &label) {
    auto *layout1 = new QHBoxLayout;

    layout->addLayout(layout1);

    auto *labelW = new QLabel(label);
    auto *edit   = new CQNewGLTextureChooser(control_);

    edit->updateWidgets();

    layout1->addWidget(labelW);
    layout1->addWidget(edit);

    return edit;
  };

#if 0
  auto createStringEdit = [&](const QString &label) {
    auto *layout1 = new QHBoxLayout;

    layout->addLayout(layout1);

    auto *labelW = new QLabel(label);
    auto *edit   = new QLineEdit;

    layout1->addWidget(labelW);
    layout1->addWidget(edit);

    return edit;
  };
#endif

  //---

  centerEdit_ = createPointEdit("Center");
  sizeEdit_   = createPointEdit("Size"  );

  translateEdit_ = createPointEdit("Translate");
  rotateEdit_    = createPointEdit("Rotate");

  diffuseMapEdit_  = createTextureEdit("Diffuse Map" );
  normalMapEdit_   = createTextureEdit("Normal Map"  );
  specularMapEdit_ = createTextureEdit("Specular Map");
  emissiveMapEdit_ = createTextureEdit("Emissive Map");

  //---

  layout->addStretch(1);

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

  diffuseMapEdit_ ->updateWidgets();
  normalMapEdit_  ->updateWidgets();
  specularMapEdit_->updateWidgets();
  emissiveMapEdit_->updateWidgets();

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

    connect(centerEdit_, SIGNAL(editingFinished()), this, SLOT(centerSlot()));
    connect(sizeEdit_  , SIGNAL(editingFinished()), this, SLOT(sizeSlot()));

    connect(translateEdit_, SIGNAL(editingFinished()), this, SLOT(translateSlot()));
    connect(rotateEdit_, SIGNAL(editingFinished()), this, SLOT(rotateSlot()));

    connect(diffuseMapEdit_ , SIGNAL(textureChanged()), this, SLOT(diffuseMapSlot()));
    connect(normalMapEdit_  , SIGNAL(textureChanged()), this, SLOT(normalMapSlot()));
    connect(specularMapEdit_, SIGNAL(textureChanged()), this, SLOT(specularMapSlot()));
    connect(emissiveMapEdit_, SIGNAL(textureChanged()), this, SLOT(emissiveMapSlot()));
  }
  else {
    disconnect(canvas(), SIGNAL(modelMatrixChanged()), this, SLOT(updateModelMatrix()));

    disconnect(objectsList_, &CQNewGLObjectsList::currentItemChanged,
               this, &CQNewGLObjectsControl::objectSelectedSlot);

    disconnect(selectButton_  , &QPushButton::clicked, this, &CQNewGLObjectsControl::selectSlot);
    disconnect(deselectButton_, &QPushButton::clicked, this, &CQNewGLObjectsControl::deselectSlot);
    disconnect(showButton_    , &QPushButton::clicked, this, &CQNewGLObjectsControl::showSlot);
    disconnect(hideButton_    , &QPushButton::clicked, this, &CQNewGLObjectsControl::hideSlot);

    disconnect(centerEdit_, SIGNAL(editingFinished()), this, SLOT(centerSlot()));
    disconnect(sizeEdit_  , SIGNAL(editingFinished()), this, SLOT(sizeSlot()));

    disconnect(translateEdit_, SIGNAL(editingFinished()), this, SLOT(translateSlot()));
    disconnect(rotateEdit_, SIGNAL(editingFinished()), this, SLOT(rotateSlot()));

    disconnect(diffuseMapEdit_ , SIGNAL(textureChanged()), this, SLOT(diffuseMapSlot()));
    disconnect(normalMapEdit_  , SIGNAL(textureChanged()), this, SLOT(normalMapSlot()));
    disconnect(specularMapEdit_, SIGNAL(textureChanged()), this, SLOT(specularMapSlot()));
    disconnect(emissiveMapEdit_, SIGNAL(textureChanged()), this, SLOT(emissiveMapSlot()));
  }
}

void
CQNewGLObjectsControl::
updateModelMatrix()
{
  connectSlots(false);

  auto tv = canvas()->modelTranslate();
  auto rv = canvas()->modelRotate();

  translateEdit_->setValue(CPoint3D(tv.x(), tv.y(), tv.z()));
  rotateEdit_   ->setValue(CPoint3D(rv.x(), rv.y(), rv.z()));

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
  const auto &scene = canvas()->importBase()->getScene();

  const auto &objects = scene.getObjects();

  auto n = int(objects.size());

  CGeomObject3D *object = nullptr;

  for (int i = 0; i < n; ++i) {
    if (i == ind)
      object = objects[i];
  }

  //---

  connectSlots(false);

  //---

  selectButton_  ->setEnabled(ind >= 0 && ! object->getSelected());
  deselectButton_->setEnabled(ind >= 0 &&   object->getSelected());

  showButton_->setEnabled(object && ! object->getVisible());
  hideButton_->setEnabled(object &&   object->getVisible());

  //---

  if (object) {
    centerEdit_ ->setValue(object->getModelCenter());
    sizeEdit_   ->setValue(object->getModelSize().point());
  }
  else {
    centerEdit_ ->setValue(CPoint3D(0, 0, 0));
    sizeEdit_   ->setValue(CPoint3D(1, 1, 1));
  }

  auto *textureMap  = (object ? object->getDiffuseTexture () : nullptr);
  auto *normalMap   = (object ? object->getNormalTexture  () : nullptr);
  auto *specularMap = (object ? object->getSpecularTexture() : nullptr);
  auto *emissiveMap = (object ? object->getEmissiveTexture() : nullptr);

  diffuseMapEdit_ ->setCurrentText(textureMap  ? QString::fromStdString(textureMap ->name()) : "");
  normalMapEdit_  ->setCurrentText(normalMap   ? QString::fromStdString(normalMap  ->name()) : "");
  specularMapEdit_->setCurrentText(specularMap ? QString::fromStdString(specularMap->name()) : "");
  emissiveMapEdit_->setCurrentText(emissiveMap ? QString::fromStdString(emissiveMap->name()) : "");

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

    canvas()->updateObjectData();
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

  if (object) {
    object->setVisible(false);

    canvas()->updateObjectData();
    canvas()->update();

    showButton_->setEnabled(true);
    hideButton_->setEnabled(false);
  }
}

void
CQNewGLObjectsControl::
centerSlot()
{
}

void
CQNewGLObjectsControl::
sizeSlot()
{
}

void
CQNewGLObjectsControl::
translateSlot()
{
  auto p = translateEdit_->getValue();

  canvas()->setModelTranslate(CVector3D(p.x, p.y, p.z));
  canvas()->update();
}

void
CQNewGLObjectsControl::
rotateSlot()
{
  auto p = rotateEdit_->getValue();

  canvas()->setModelRotate(CVector3D(p.x, p.y, p.z));
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

    canvas()->updateObjectData();
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

    canvas()->updateObjectData();
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

    canvas()->updateObjectData();
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

    canvas()->updateObjectData();
    canvas()->update();
  }
}

CGeomObject3D *
CQNewGLObjectsControl::
getSelectedObject() const
{
  const auto &scene = canvas()->importBase()->getScene();

  const auto &objects = scene.getObjects();

  auto n = int(objects.size());

  for (int i = 0; i < n; ++i) {
    if (objects[i]->getSelected())
      return objects[i];
  }

  return nullptr;
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
updateWidgets()
{
  connectSlots(false);

  auto *canvas = this->canvas();

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

  CGLTexture *texture = nullptr;

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

  auto *qimage = dynamic_cast<CQImage *>(texture->getImage().get());

  auto qimage1 = qimage->getQImage().scaled(256, 256,
    Qt::KeepAspectRatio, Qt::FastTransformation);

  p.drawImage(0, 0, qimage1);
}


//---

CQNewGLTextureChooser::
CQNewGLTextureChooser(CQNewGLControl *control) :
 control_(control)
{
  connectSlots(true);
}

void
CQNewGLTextureChooser::
updateWidgets()
{
  connectSlots(false);

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

  auto addLabelEdit = [&](const QString &label, QWidget *w) {
    auto *frame   = new QFrame;
    auto *layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    layout->addWidget(frame);
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

//objectsList_ = new CQNewGLObjectsList(control);

//layout->addWidget(objectsList_);

  //---

  nameCombo_ = new QComboBox;
  addLabelEdit("Name", nameCombo_);

  //---

  timeEdit_ = new CQRealSpin;
  addLabelEdit("Time", timeEdit_);

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

  timer_ = new QTimer;

  connect(timer_, &QTimer::timeout, this, &CQNewGLAnimControl::timerSlot);
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
  canvas->updateObjectData();
  canvas->update();
}

void
CQNewGLAnimControl::
bonesSlot(int state)
{
  auto *canvas = control_->canvas();

  canvas->setBonesEnabled(state);
  canvas->updateObjectData();
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

  canvas->updateObjectData();
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
  canvas->updateObjectData();
  canvas->update();
}

void
CQNewGLAnimControl::
timeSlot(double t)
{
  auto *canvas = control_->canvas();

  canvas->setTime(t);
  canvas->update();
}

void
CQNewGLAnimControl::
playSlot()
{
  step();

  timer_->start(100);
}

void
CQNewGLAnimControl::
pauseSlot()
{
  timer_->stop();
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
  const auto &scene = canvas()->importBase()->getScene();

  const auto &objects = scene.getObjects();

  auto n = int(objects.size());

  for (int i = 0; i < n; ++i) {
    if (! objects[i]->getNodes().empty())
      return objects[i];
  }

  return nullptr;
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

    canvas->updateObjectData();
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

CQNewGLObjectsList::
CQNewGLObjectsList(CQNewGLControl *control) :
 QFrame(nullptr), control_(control)
{
  auto *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(list_);

  //---

  connectSlots(true);
}

void
CQNewGLObjectsList::
updateObjects()
{
  auto *canvas = control_->canvas();

  auto &scene = canvas->importBase()->getScene();

  list_->clear();

  int ind = -1;

  auto *item = new QListWidgetItem("");

  list_->addItem(item);

  item->setData(Qt::UserRole, ind++);

  for (auto *object : scene.getObjects()) {
    auto objectName = QString::fromStdString(object->getName());

    auto *item = new QListWidgetItem(objectName);

    list_->addItem(item);

    item->setData(Qt::UserRole, ind++);
  }
}

void
CQNewGLObjectsList::
connectSlots(bool b)
{
  if (b)
    connect(list_, &QListWidget::currentItemChanged,
            this, &CQNewGLObjectsList::objectSelectedSlot);
  else
    disconnect(list_, &QListWidget::currentItemChanged,
               this, &CQNewGLObjectsList::objectSelectedSlot);
}

void
CQNewGLObjectsList::
objectSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

CGeomObject3D *
CQNewGLObjectsList::
getSelectedIndObject() const
{
  auto *canvas = control_->canvas();

  const auto &scene = canvas->importBase()->getScene();

  const auto &objects = scene.getObjects();

  auto n = int(objects.size());

  for (int i = 0; i < n; ++i) {
    if (i == selectedInd_)
      return objects[i];
  }

  return nullptr;
}

CGeomObject3D *
CQNewGLObjectsList::
getObjectListSelected() const
{
  auto *canvas = control_->canvas();

  auto selectedItems = list_->selectedItems();

  int ind = -1;

  for (const auto *item : selectedItems) {
    int ind1 = item->data(Qt::UserRole).toInt();
    if (ind1 >= 0) {
      ind = ind1;
      break;
    }
  }

  const auto &scene = canvas->importBase()->getScene();

  const auto &objects = scene.getObjects();

  auto n = int(objects.size());

  for (int i = 0; i < n; ++i) {
    if (i == ind)
      return objects[i];
  }

  return nullptr;
}
