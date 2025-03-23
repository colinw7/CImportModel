#include <CQNewGLControl.h>
#include <CQNewGLModel.h>
#include <CQNewGLCanvas.h>

#include <CImportBase.h>
#include <CGeomScene3D.h>
#include <CGLCamera.h>

#include <CQColorEdit.h>
#include <CQRealSpin.h>
#include <CQSlider.h>
#include <CQPoint3DEdit.h>

#include <QTabWidget>
#include <QListWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QVBoxLayout>

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

  cameraControl_ = new CQNewGLCameraControl(this);

  tab_->addTab(cameraControl_, "Camera");

  lightControl_ = new CQNewGLLightControl(this);

  tab_->addTab(lightControl_, "Lights");

  objectsControl_ = new CQNewGLObjectsControl(this);

  tab_->addTab(objectsControl_, "Objects");

  //---

  layout->addStretch(1);
}

void
CQNewGLControl::
update()
{
  objectsControl_->update();
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

  colorEdit_ = new CQColorEdit(this),

  colorEdit_->setColor(canvas->bgColor());

  connect(colorEdit_, SIGNAL(colorChanged(const QColor &)),
          this, SLOT(bgColorSlot(const QColor &)));

  layout->addWidget(colorEdit_);

  //---

  depthTestCheck_ = new QCheckBox("Depth Test");

  depthTestCheck_->setChecked(canvas->isDepthTest());

  connect(depthTestCheck_, SIGNAL(stateChanged(int)), this, SLOT(depthTestSlot(int)));

  layout->addWidget(depthTestCheck_);

  //---

  cullCheck_ = new QCheckBox("Cull Face");

  cullCheck_->setChecked(canvas->isCullFace());

  connect(cullCheck_, SIGNAL(stateChanged(int)), this, SLOT(cullSlot(int)));

  layout->addWidget(cullCheck_);

  //---

  frontFaceCheck_ = new QCheckBox("Front Face");

  frontFaceCheck_->setChecked(canvas->isFrontFace());

  connect(frontFaceCheck_, SIGNAL(stateChanged(int)), this, SLOT(frontFaceSlot(int)));

  layout->addWidget(frontFaceCheck_);

  //---

  polygonLineCheck_ = new QCheckBox("Polygon Line");

  polygonLineCheck_->setChecked(canvas->isPolygonLine());

  connect(polygonLineCheck_, SIGNAL(stateChanged(int)), this, SLOT(polygonLineSlot(int)));

  layout->addWidget(polygonLineCheck_);

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

  auto *ambientSlider   = createSlider("Ambient"  , 0.0,   1.0, app->ambient  (),
                                       SLOT(ambientSlot(double)));
  auto *diffuseSlider   = createSlider("Diffuse"  , 0.0,   1.0, app->diffuse  (),
                                       SLOT(diffuseSlot(double)));
  auto *specularSlider  = createSlider("Specular" , 0.0,   1.0, app->specular (),
                                       SLOT(specularSlot(double)));
  auto *shininessSlider = createSlider("Shininess", 0.0, 100.0, app->shininess(),
                                       SLOT(shininessSlot(double)));

  layout->addWidget(ambientSlider);
  layout->addWidget(diffuseSlider);
  layout->addWidget(specularSlider);
  layout->addWidget(shininessSlider);

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

  layout->addWidget(modelCheck);
  layout->addWidget(lightCheck);
  layout->addWidget(cameraCheck);

  layout->addStretch(1);
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
polygonLineSlot(int state)
{
  auto *canvas = this->canvas();

  canvas->setPolygonLine(state);
  canvas->update();
}

void
CQNewGLGeneralControl::
ambientSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app   ->setAmbient(v);
  canvas->update();
}

void
CQNewGLGeneralControl::
diffuseSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app   ->setDiffuse(v);
  canvas->update();
}

void
CQNewGLGeneralControl::
specularSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app   ->setSpecular(v);
  canvas->update();
}

void
CQNewGLGeneralControl::
shininessSlot(double v)
{
  auto *app    = control_->app();
  auto *canvas = this->canvas();

  app   ->setShininess(v);
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
  connect(rotateCheck_, &QCheckBox::stateChanged, this, &CQNewGLCameraControl::rotateSlot);
  addLabelEdit("Rotate", rotateCheck_);

  zoomEdit_ = new CQRealSpin;
  connect(zoomEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::zoomSlot);
  addLabelEdit("Zoom", zoomEdit_);

  nearEdit_ = new CQRealSpin;
  connect(nearEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::nearSlot);
  addLabelEdit("Near", nearEdit_);

  farEdit_ = new CQRealSpin;
  connect(farEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::farSlot);
  addLabelEdit("Far", farEdit_);

  yawEdit_ = new CQRealSpin;
  connect(yawEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::yawSlot);
  addLabelEdit("Yaw", yawEdit_);

  pitchEdit_ = new CQRealSpin;
  connect(pitchEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::pitchSlot);
  addLabelEdit("Pitch", pitchEdit_);

  rollEdit_ = new CQRealSpin;
  connect(rollEdit_, &CQRealSpin::realValueChanged, this, &CQNewGLCameraControl::rollSlot);
  addLabelEdit("Roll", rollEdit_);

  posEdit_ = new CQPoint3DEdit;
  connect(posEdit_, &CQPoint3DEdit::editingFinished, this, &CQNewGLCameraControl::posSlot);
  addLabelEdit("Position", posEdit_);

  //---

  layout->setRowStretch(row, 1);
}

CGLCamera *
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

//---

CQNewGLLightControl::
CQNewGLLightControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
}

//---

CQNewGLObjectsControl::
CQNewGLObjectsControl(CQNewGLControl *control) :
 CQNewGLControlFrame(control)
{
  auto *layout = new QVBoxLayout(this);

  objectsList_ = new QListWidget;

  objectsList_->setSelectionMode(QListWidget::SingleSelection);

  connect(objectsList_, &QListWidget::currentItemChanged,
          this, &CQNewGLObjectsControl::objectSelectedSlot);

  layout->addWidget(objectsList_);

#if 0
  objectTree_ = new CQPropertyViewTree(this);

  layout->addWidget(objectTree_);
#endif

  //---

  auto createEdit = [&](const QString &label, const char *slotName) {
    auto *layout1 = new QHBoxLayout;

    layout->addLayout(layout1);

    auto *labelW = new QLabel(label);
    auto *edit   = new CQPoint3DEdit;

    layout1->addWidget(labelW);
    layout1->addWidget(edit);

    connect(edit, SIGNAL(editingFinished()), this, slotName);

    return edit;
  };

  //---

  centerEdit_ = createEdit("Center", SLOT(centerSlot()));
  sizeEdit_   = createEdit("Size"  , SLOT(sizeSlot()));

  //---

  layout->addStretch(1);
}

void
CQNewGLObjectsControl::
update()
{
  disconnect(objectsList_, &QListWidget::currentItemChanged,
             this, &CQNewGLObjectsControl::objectSelectedSlot);

  disconnect(centerEdit_, SIGNAL(editingFinished()), this, SLOT(centerSlot()));
  disconnect(sizeEdit_  , SIGNAL(editingFinished()), this, SLOT(sizeSlot()));

  //---

  auto &scene = canvas()->importBase()->getScene();

  objectsList_->clear();

  int ind = 0;

  for (auto *object : scene.getObjects()) {
    auto objectName = QString::fromStdString(object->getName());

    auto *item = new QListWidgetItem(objectName);

    objectsList_->addItem(item);

    item->setData(Qt::UserRole, ind++);
  }

  //---

  connect(objectsList_, &QListWidget::currentItemChanged,
          this, &CQNewGLObjectsControl::objectSelectedSlot);

  connect(centerEdit_, SIGNAL(editingFinished()), this, SLOT(centerSlot()));
  connect(sizeEdit_  , SIGNAL(editingFinished()), this, SLOT(sizeSlot()));
}

void
CQNewGLObjectsControl::
objectSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  int ind = item->data(Qt::UserRole).toInt();

  std::cerr << ind << "\n";

  const auto &scene = canvas()->importBase()->getScene();

  const auto &objects = scene.getObjects();

  if (ind < 0 || ind >= int(objects.size()))
    return;

  auto &object = objects[ind];

  centerEdit_->setValue(object->getModelCenter());
  sizeEdit_  ->setValue(object->getModelSize().point());
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
  return control_->app()->canvas();
}

