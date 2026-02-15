#include <LightControl.h>
#include <MaterialView.h>
#include <Light.h>
#include <App.h>
#include <UI.h>
#include <Util.h>

#include <CQRealSpin.h>
#include <CQColorEdit.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QCheckBox>
#include <QListWidget>
#include <QVBoxLayout>

namespace {

class LightTypeInd : public ValueMap<CGeomLight3DType, int> {
 public:
  LightTypeInd() {
    add("Directional", CGeomLight3DType::DIRECTIONAL, 0);
    add("Point"      , CGeomLight3DType::POINT      , 1);
    add("Spot"       , CGeomLight3DType::SPOT       , 2);
  };

  CGeomLight3DType indToType(int ind) {
    return lookup(ind);
  }

  int typeToInd(const CGeomLight3DType &type) {
    return lookup(type);
  }
};

LightTypeInd lightTypeInd;

}

//---

namespace CQTextureGen {

LightControl::
LightControl(App *app) :
 app_(app)
{
  setObjectName("lightControl");

//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto *layout = new QVBoxLayout(this);

  UI ui(this, layout);

  //---

  ui.startGroup("Ambient");

  ambientColorEdit_    = ui.addLabelEdit("Color"   , new CQColorEdit);
  ambientStrengthEdit_ = ui.addLabelEdit("Strength", new CQRealSpin);

  ui.endGroup();

  //---

  ui.startGroup("Diffuse");

  diffuseStrengthEdit_ = ui.addLabelEdit("Strength", new CQRealSpin);

  ui.endGroup();

  //---

  ui.startGroup("Specular");

  specularColorEdit_    = ui.addLabelEdit("Color"   , new CQColorEdit);
  specularStrengthEdit_ = ui.addLabelEdit("Strength", new CQRealSpin);

  ui.endGroup();

  //---

  ui.startGroup("Emission");

  emissionColorEdit_    = ui.addLabelEdit("Color"   , new CQColorEdit);
  emissionStrengthEdit_ = ui.addLabelEdit("Strength", new CQRealSpin);

  ui.endGroup();

  shininessEdit_ = ui.addLabelEdit("Shininess", new CQRealSpin);

  //---

  lightList_ = ui.addWidgetT(new LightList(this));

  //---

  enabledCheck_ = ui.addLabelEdit("Enabled", new QCheckBox);

  typeCombo_ = ui.addLabelEdit("Type", new QComboBox);
  typeCombo_->addItems(lightTypeInd.names());

  colorEdit_ = ui.addLabelEdit("Color", new CQColorEdit);

  ui.startGroup("Position");

  xPosEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  yPosEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  zPosEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  directionGroup_ = ui.startGroup("Direction");

  xDirEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  yDirEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  zDirEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  pointGroup_ = ui.startGroup("Point");

  radiusEdit_ = ui.addLabelEdit("Radius", new CQRealSpin);

  ui.startGroup("Attenuation");

  attenuation0Edit_ = ui.addLabelEdit("Constant" , new CQRealSpin);
  attenuation1Edit_ = ui.addLabelEdit("Linear"   , new CQRealSpin);
  attenuation2Edit_ = ui.addLabelEdit("Quadratic", new CQRealSpin);

  ui.endGroup();

  ui.endGroup();

  spotGroup_ = ui.startGroup("Spot");

  expEdit_    = ui.addLabelEdit("Exponent", new CQRealSpin);
  cutOffEdit_ = ui.addLabelEdit("Cut Off", new CQRealSpin);

  ui.startGroup("Direction");

  spotXDirEdit_ = ui.addLabelEdit("X", new CQRealSpin);
  spotYDirEdit_ = ui.addLabelEdit("Y", new CQRealSpin);
  spotZDirEdit_ = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.endGroup();

  ui.addStretch();

  ui.startHFrame();

  auto *addButton = ui.addWidget(new QPushButton("Add"));

  connect(addButton, SIGNAL(clicked()), this, SLOT(addSlot()));

  ui.addStretch();

  ui.endFrame();

  connectSlots(true);

  updateWidgets();
}

void
LightControl::
updateWidgets()
{
  connectSlots(false);

  auto *materialView = app_->materialView();

  ambientColorEdit_    ->setColor(RGBAToQColor(materialView->ambientColor()));
  ambientStrengthEdit_ ->setValue(materialView->ambientStrength());
  diffuseStrengthEdit_ ->setValue(materialView->diffuseStrength());
  specularColorEdit_   ->setColor(RGBAToQColor(materialView->specularColor()));
  specularStrengthEdit_->setValue(materialView->specularStrength());
  emissionColorEdit_   ->setColor(RGBAToQColor(materialView->emissionColor()));
  emissionStrengthEdit_->setValue(materialView->emissionStrength());
  shininessEdit_       ->setValue(materialView->shininess());

  lightList_->updateLights();

  auto *light = currentLight();

  enabledCheck_->setChecked(light->isEnabled());

  typeCombo_->setCurrentIndex(lightTypeInd.typeToInd(light->type()));

  colorEdit_->setColor(RGBAToQColor(light->color()));

  xPosEdit_->setValue(light->position().x);
  yPosEdit_->setValue(light->position().y);
  zPosEdit_->setValue(light->position().z);

  directionGroup_->setVisible(light->type() == Light::Type::DIRECTIONAL);

  xDirEdit_->setValue(light->direction().x());
  yDirEdit_->setValue(light->direction().y());
  zDirEdit_->setValue(light->direction().z());

  pointGroup_->setVisible(light->type() == Light::Type::POINT);

  radiusEdit_->setValue(light->getPointRadius());

  attenuation0Edit_->setValue(light->getConstantAttenuation());
  attenuation1Edit_->setValue(light->getLinearAttenuation());
  attenuation2Edit_->setValue(light->getQuadraticAttenuation());

  spotGroup_->setVisible(light->type() == Light::Type::SPOT);

  expEdit_     ->setValue(light->getSpotExponent());
  cutOffEdit_  ->setValue(light->getSpotCutOffAngle());
  spotXDirEdit_->setValue(light->getSpotDirection().x());
  spotYDirEdit_->setValue(light->getSpotDirection().y());
  spotZDirEdit_->setValue(light->getSpotDirection().z());

  connectSlots(true);
}

void
LightControl::
connectSlots(bool b)
{
  auto *materialView = app_->materialView();

  CQUtil::connectDisconnect(b, materialView, SIGNAL(lightAdded()),
                            this, SLOT(updateWidgets()));
  CQUtil::connectDisconnect(b, materialView, SIGNAL(lightChanged()),
                            this, SLOT(updateWidgets()));

  CQUtil::connectDisconnect(b, ambientColorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(ambientColorSlot(const QColor &)));
  CQUtil::connectDisconnect(b, ambientStrengthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(ambientStrengthSlot(double)));
  CQUtil::connectDisconnect(b, diffuseStrengthEdit_ , SIGNAL(realValueChanged(double)),
                            this, SLOT(diffuseStrengthSlot(double)));
  CQUtil::connectDisconnect(b, specularColorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(specularColorSlot(const QColor &)));
  CQUtil::connectDisconnect(b, specularStrengthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(specularStrengthSlot(double)));
  CQUtil::connectDisconnect(b, emissionColorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(emissionColorSlot(const QColor &)));
  CQUtil::connectDisconnect(b, emissionStrengthEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(emissionStrengthSlot(double)));
  CQUtil::connectDisconnect(b, shininessEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(shininessSlot(double)));

  CQUtil::connectDisconnect(b, lightList_, SIGNAL(currentItemChanged()),
                            this, SLOT(updateWidgets()));

  CQUtil::connectDisconnect(b, enabledCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(enabledSlot(int)));

  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));

  CQUtil::connectDisconnect(b, colorEdit_, SIGNAL(colorChanged(const QColor &)),
                            this, SLOT(colorSlot(const QColor &)));

  CQUtil::connectDisconnect(b, xPosEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xPosSlot(double)));
  CQUtil::connectDisconnect(b, yPosEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(yPosSlot(double)));
  CQUtil::connectDisconnect(b, zPosEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zPosSlot(double)));

  CQUtil::connectDisconnect(b, xDirEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(xDirSlot(double)));
  CQUtil::connectDisconnect(b, yDirEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(yDirSlot(double)));
  CQUtil::connectDisconnect(b, zDirEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(zDirSlot(double)));

  CQUtil::connectDisconnect(b, radiusEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(pointRadiusSlot(double)));
  CQUtil::connectDisconnect(b, attenuation0Edit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(attenuation0Slot(double)));
  CQUtil::connectDisconnect(b, attenuation1Edit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(attenuation1Slot(double)));
  CQUtil::connectDisconnect(b, attenuation2Edit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(attenuation2Slot(double)));

  CQUtil::connectDisconnect(b, spotXDirEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(spotXDirSlot(double)));
  CQUtil::connectDisconnect(b, spotYDirEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(spotYDirSlot(double)));
  CQUtil::connectDisconnect(b, spotZDirEdit_, SIGNAL(realValueChanged(double)),
                            this, SLOT(spotZDirSlot(double)));
}

void
LightControl::
ambientColorSlot(const QColor &c)
{
  auto *materialView = app_->materialView();

  materialView->setAmbientColor(QColorToRGBA(c));
}

void
LightControl::
ambientStrengthSlot(double r)
{
  auto *materialView = app_->materialView();

  materialView->setAmbientStrength(r);
}

void
LightControl::
diffuseStrengthSlot(double r)
{
  auto *materialView = app_->materialView();

  materialView->setDiffuseStrength(r);
}

void
LightControl::
specularColorSlot(const QColor &c)
{
  auto *materialView = app_->materialView();

  materialView->setSpecularColor(QColorToRGBA(c));
}

void
LightControl::
specularStrengthSlot(double r)
{
  auto *materialView = app_->materialView();

  materialView->setSpecularStrength(r);
}

void
LightControl::
emissionColorSlot(const QColor &c)
{
  auto *materialView = app_->materialView();

  materialView->setEmissionColor(QColorToRGBA(c));
}

void
LightControl::
emissionStrengthSlot(double r)
{
  auto *materialView = app_->materialView();

  materialView->setEmissionStrength(r);
}

void
LightControl::
shininessSlot(double r)
{
  auto *materialView = app_->materialView();

  materialView->setShininess(r);
}

void
LightControl::
enabledSlot(int state)
{
  auto *light = currentLight();

  light->setEnabled(state);
}

void
LightControl::
typeSlot(int ind)
{
  auto *light = currentLight();

  light->setType(lightTypeInd.indToType(ind));
}

void
LightControl::
colorSlot(const QColor &c)
{
  auto *light = currentLight();

  light->setColor(QColorToRGBA(c));
}

void
LightControl::
xPosSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto pos = light->position();

  pos.x = r;

  light->setPosition(pos);

  connectSlots(true);
}

void
LightControl::
yPosSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto pos = light->position();

  pos.y = r;

  light->setPosition(pos);

  connectSlots(true);
}

void
LightControl::
zPosSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto pos = light->position();

  pos.z = r;

  light->setPosition(pos);

  connectSlots(true);
}

void
LightControl::
xDirSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto dir = light->direction();

  dir.setX(r);

  light->setDirection(dir);

  connectSlots(true);
}

void
LightControl::
yDirSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto dir = light->direction();

  dir.setY(r);

  light->setDirection(dir);

  connectSlots(true);
}

void
LightControl::
zDirSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto dir = light->direction();

  dir.setZ(r);

  light->setDirection(dir);

  connectSlots(true);
}

void
LightControl::
pointRadiusSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  light->setPointRadius(r);

  connectSlots(true);
}

void
LightControl::
attenuation0Slot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  light->setConstantAttenuation(r);

  connectSlots(true);
}

void
LightControl::
attenuation1Slot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  light->setLinearAttenuation(r);

  connectSlots(true);
}

void
LightControl::
attenuation2Slot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  light->setQuadraticAttenuation(r);

  connectSlots(true);
}

void
LightControl::
spotXDirSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto dir = light->getSpotDirection();

  dir.setX(r);

  light->setSpotDirection(dir);

  connectSlots(true);
}

void
LightControl::
spotYDirSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto dir = light->getSpotDirection();

  dir.setY(r);

  light->setSpotDirection(dir);

  connectSlots(true);
}

void
LightControl::
spotZDirSlot(double r)
{
  connectSlots(false);

  auto *light = currentLight();

  auto dir = light->getSpotDirection();

  dir.setZ(r);

  light->setSpotDirection(dir);

  connectSlots(true);
}

void
LightControl::
addSlot()
{
  auto *materialView = app_->materialView();

  materialView->addLight();
}

Light *
LightControl::
currentLight() const
{
  auto *light = lightList_->getCurrentLight();

  if (! light) {
    auto *materialView = app_->materialView();

    light = materialView->lights()[0];
  }

  return light;
}

//---

LightList::
LightList(LightControl *control) :
 control_(control)
{
  setObjectName("materialList");

  auto *app          = control_->app();
  auto *materialView = app->materialView();

  connect(materialView, SIGNAL(lightAdded()), this, SLOT(needsUpdateSlot()));

  //---

  auto *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);

  layout->addWidget(list_);

  //---

  connectSlots(true);
}

void
LightList::
needsUpdateSlot()
{
  valid_ = false;

  updateLights();
}

void
LightList::
updateLights()
{
  if (valid_)
    return;

  valid_ = true;

  //---

  connectSlots(false);

  //---

  list_->clear();

  auto *app          = control_->app();
  auto *materialView = app->materialView();

  auto *item = new QListWidgetItem("");

  list_->addItem(item);

  item->setData(Qt::UserRole, -1);

  for (auto *light : materialView->lights()) {
    auto name = QString::fromStdString(light->name());

    auto *item = new QListWidgetItem(name);

    list_->addItem(item);

    item->setData(Qt::UserRole, light->id());
  }

  selectedInd_ = -1;

  //---

  connectSlots(true);
}

void
LightList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    list_, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    this, SLOT(itemSelectedSlot(QListWidgetItem *, QListWidgetItem *)));
}

void
LightList::
itemSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);

  Q_EMIT currentItemChanged();
}

Light *
LightList::
getCurrentLight() const
{
  if (selectedInd_ < 0)
    return nullptr;

  auto *app          = control_->app();
  auto *materialView = app->materialView();

  return materialView->getLightById(selectedInd_);
}

Light *
LightList::
getSelectedLight() const
{
  auto selectedItems = list_->selectedItems();

  int ind = -1;

  for (const auto *item : selectedItems) {
    int ind1 = item->data(Qt::UserRole).toInt();
    if (ind1 >= 0) {
      ind = ind1;
      break;
    }
  }

  if (ind < 0)
    return nullptr;

  auto *app          = control_->app();
  auto *materialView = app->materialView();

  return materialView->getLightById(ind);
}

//---

}

