#include <Control.h>
#include <Canvas.h>
#include <Camera.h>
#include <Light.h>
#include <Overview.h>
#include <App.h>
#include <Util.h>
#include <UI.h>

#include <CQPoint3DEdit.h>
#include <CQRealSpin.h>

#include <QCheckBox>
#include <QComboBox>

namespace {

class LightTypeInd : public CQShadow3D::ValueMap<CGeomLight3DType, int> {
 public:
  LightTypeInd() {
    add("Directional", CGeomLight3DType::DIRECTIONAL, 0);
    add("Point"      , CGeomLight3DType::POINT      , 1);
    add("Spot"       , CGeomLight3DType::SPOT       , 2);
    add("Flashlight" , CGeomLight3DType::FLASHLIGHT , 3);
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

namespace CQShadow3D {

Control::
Control(App *app) :
 app_(app)
{
  setObjectName("control");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  UI ui(this, layout);

  //---

  ui.startTab("tab");

  //---

  ui.startTabPage("General");

  ui.startGroup("Skybox");

  generalData_.skyboxCheck = ui.addLabelEdit("Visible", new QCheckBox);

  ui.endGroup();

  ui.startGroup("Shadow Map");

  generalData_.shadowMapDebug = ui.addLabelEdit("Debug", new QCheckBox);

  ui.endGroup();

  ui.addStretch();

  ui.endTabPage();

  //---

  ui.startTabPage("Camera");

  cameraData_.disableRollCheck = ui.addLabelEdit("Disable Roll", new QCheckBox);

  ui.startGroup("Clamp Pitch");

  cameraData_.clampPitchCheck = ui.addLabelEdit("Enabled", new QCheckBox);
  cameraData_.minPitchEdit    = ui.addLabelEdit("Min"    , new CQRealSpin);
  cameraData_.maxPitchEdit    = ui.addLabelEdit("Max"    , new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Clamp Yaw");

  cameraData_.clampYawCheck = ui.addLabelEdit("Yaw", new QCheckBox);
  cameraData_.minYawEdit    = ui.addLabelEdit("Min", new CQRealSpin);
  cameraData_.maxYawEdit    = ui.addLabelEdit("Max", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Clamp Roll");

  cameraData_.clampRollCheck = ui.addLabelEdit("Roll", new QCheckBox);
  cameraData_.minRollEdit    = ui.addLabelEdit("Min" , new CQRealSpin);
  cameraData_.maxRollEdit    = ui.addLabelEdit("Max" , new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Origin");

  cameraData_.xOriginEdit = ui.addLabelEdit("X", new CQRealSpin);
  cameraData_.yOriginEdit = ui.addLabelEdit("Y", new CQRealSpin);
  cameraData_.zOriginEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Position");

  cameraData_.xPosEdit = ui.addLabelEdit("X", new CQRealSpin);
  cameraData_.yPosEdit = ui.addLabelEdit("Y", new CQRealSpin);
  cameraData_.zPosEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Direction");

  cameraData_.xDirEdit = ui.addLabelEdit("X", new CQRealSpin);
  cameraData_.yDirEdit = ui.addLabelEdit("Y", new CQRealSpin);
  cameraData_.zDirEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  cameraData_.distanceEdit = ui.addLabelEdit("Distance", new CQRealSpin);

  ui.startGroup("Angles");

  cameraData_.pitchEdit = ui.addLabelEdit("Pitch", new CQRealSpin);
  cameraData_.yawEdit   = ui.addLabelEdit("Yaw"  , new CQRealSpin);
  cameraData_.rollEdit  = ui.addLabelEdit("Roll" , new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Z");

  cameraData_.nearEdit = ui.addLabelEdit("Near", new CQRealSpin);
  cameraData_.farEdit  = ui.addLabelEdit("Far" , new CQRealSpin);
  cameraData_.fovEdit  = ui.addLabelEdit("FOV" , new CQRealSpin);

  ui.endGroup();

  ui.addStretch();

  ui.endTabPage();

  //---

  ui.startTabPage("Light");

  lightData_.typeCombo = ui.addLabelEdit("Type", new QComboBox);
  lightData_.typeCombo->addItems(lightTypeInd.names());

  ui.startGroup("Origin");

  lightData_.xOriginEdit = ui.addLabelEdit("X", new CQRealSpin);
  lightData_.yOriginEdit = ui.addLabelEdit("Y", new CQRealSpin);
  lightData_.zOriginEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Position");

  lightData_.xPosEdit = ui.addLabelEdit("X", new CQRealSpin);
  lightData_.yPosEdit = ui.addLabelEdit("Y", new CQRealSpin);
  lightData_.zPosEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Direction");

  lightData_.xDirEdit = ui.addLabelEdit("X", new CQRealSpin);
  lightData_.yDirEdit = ui.addLabelEdit("Y", new CQRealSpin);
  lightData_.zDirEdit = ui.addLabelEdit("Z", new CQRealSpin);

  ui.endGroup();

  ui.startGroup("Z");

  lightData_.nearEdit = ui.addLabelEdit("Near", new CQRealSpin);
  lightData_.farEdit  = ui.addLabelEdit("Far" , new CQRealSpin);
  lightData_.fovEdit  = ui.addLabelEdit("FOV" , new CQRealSpin);

  ui.endGroup();

  lightData_.shadowBiasEdit  = ui.addLabelEdit("Bias" , new CQRealSpin);
//lightData_.shadowScaleEdit = ui.addLabelEdit("Scale", new CQRealSpin);

  // direction light
  ui.startGroup("Directional");

  lightData_.directionEdit = ui.addLabelEdit("Direction", new CQPoint3DEdit);

  ui.endGroup();

  // point light
  ui.startGroup("Point");

  lightData_.pointRadiusEdit = ui.addLabelEdit("Radius", new CQRealSpin);

  lightData_.attenuation0Edit = ui.addLabelEdit("Constant Attenuation", new CQRealSpin);
  lightData_.attenuation1Edit = ui.addLabelEdit("Linear Attenuation", new CQRealSpin);
  lightData_.attenuation2Edit = ui.addLabelEdit("Quadratic Attenuation", new CQRealSpin);

  ui.endGroup();

  // spot light
  ui.startGroup("Spot");

  lightData_.spotDirectionEdit    = ui.addLabelEdit("Direction", new CQPoint3DEdit);
  lightData_.spotExponentEdit     = ui.addLabelEdit("Exponent", new CQRealSpin);
  lightData_.spotCutOffAngle      = ui.addLabelEdit("Cut Off", new CQRealSpin);
  lightData_.spotOuterCutOffAngle = ui.addLabelEdit("Outer Cut Off", new CQRealSpin);

  ui.endGroup();

  ui.addButton("Sync", SLOT(syncSlot()));

  ui.addStretch();

  ui.endTabPage();

  //---

  ui.startTabPage("Overview");

  overviewData_.cameraCheck = ui.addLabelEdit("Camera", new QCheckBox);
  overviewData_.lightCheck  = ui.addLabelEdit("Light" , new QCheckBox);

  ui.addStretch();

  ui.endTabPage();

  //---

  ui.endTab();

  //---

  connectSlots(true);

  //---

  updateWidgets();

  connect(camera(), SIGNAL(stateChangedSignal()), this, SLOT(updateWidgets()));

  connect(light(), SIGNAL(stateChangedSignal()), this, SLOT(updateWidgets()));
}

void
Control::
connectSlots(bool b)
{
  auto connectCheckBox = [&](QCheckBox *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(stateChanged(int)), this, slotName);
  };

  auto connectRealSpin = [&](CQRealSpin *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(realValueChanged(double)), this, slotName);
  };

  auto connectComboBox = [&](QComboBox *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(currentIndexChanged(int)), this, slotName);
  };

  auto connectPointEdit = [&](CQPoint3DEdit *w, const char *slotName) {
    CQUtil::connectDisconnect(b, w, SIGNAL(editingFinished()), this, slotName);
  };

  //---

  connectCheckBox(generalData_.skyboxCheck, SLOT(skyboxVisibleSlot(int)));
  connectCheckBox(generalData_.shadowMapDebug, SLOT(shadowMapDebugSlot(int)));

  //---

  connectCheckBox(cameraData_.disableRollCheck, SLOT(disableRollSlot(int)));

  connectCheckBox(cameraData_.clampPitchCheck, SLOT(clampPitchSlot(int)));
  connectRealSpin(cameraData_.minPitchEdit   , SLOT(minPitchSlot(double)));
  connectRealSpin(cameraData_.maxPitchEdit   , SLOT(maxPitchSlot(double)));

  connectCheckBox(cameraData_.clampYawCheck, SLOT(clampYawSlot(int)));
  connectRealSpin(cameraData_.minYawEdit   , SLOT(minYawSlot(double)));
  connectRealSpin(cameraData_.maxYawEdit   , SLOT(maxYawSlot(double)));

  connectCheckBox(cameraData_.clampRollCheck, SLOT(clampRollSlot(int)));
  connectRealSpin(cameraData_.minRollEdit   , SLOT(minRollSlot(double)));
  connectRealSpin(cameraData_.maxRollEdit   , SLOT(maxRollSlot(double)));

  connectRealSpin(cameraData_.xOriginEdit, SLOT(cameraXOriginSlot(double)));
  connectRealSpin(cameraData_.yOriginEdit, SLOT(cameraYOriginSlot(double)));
  connectRealSpin(cameraData_.zOriginEdit, SLOT(cameraZOriginSlot(double)));

  connectRealSpin(cameraData_.xPosEdit, SLOT(cameraXPosSlot(double)));
  connectRealSpin(cameraData_.yPosEdit, SLOT(cameraYPosSlot(double)));
  connectRealSpin(cameraData_.zPosEdit, SLOT(cameraZPosSlot(double)));

  connectRealSpin(cameraData_.xDirEdit, SLOT(cameraXDirSlot(double)));
  connectRealSpin(cameraData_.yDirEdit, SLOT(cameraYDirSlot(double)));
  connectRealSpin(cameraData_.zDirEdit, SLOT(cameraZDirSlot(double)));

  connectRealSpin(cameraData_.distanceEdit, SLOT(distanceSlot(double)));

  connectRealSpin(cameraData_.pitchEdit, SLOT(pitchSlot(double)));
  connectRealSpin(cameraData_.yawEdit  , SLOT(yawSlot(double)));
  connectRealSpin(cameraData_.rollEdit , SLOT(rollSlot(double)));

  connectRealSpin(cameraData_.nearEdit, SLOT(cameraNearSlot(double)));
  connectRealSpin(cameraData_.farEdit , SLOT(cameraFarSlot(double)));
  connectRealSpin(cameraData_.fovEdit , SLOT(cameraFovSlot(double)));

  //---

  connectComboBox(lightData_.typeCombo, SLOT(lightTypeSlot(int)));

  connectRealSpin(lightData_.xOriginEdit, SLOT(lightXOriginSlot(double)));
  connectRealSpin(lightData_.yOriginEdit, SLOT(lightYOriginSlot(double)));
  connectRealSpin(lightData_.zOriginEdit, SLOT(lightZOriginSlot(double)));

  connectRealSpin(lightData_.xPosEdit, SLOT(lightXPosSlot(double)));
  connectRealSpin(lightData_.yPosEdit, SLOT(lightYPosSlot(double)));
  connectRealSpin(lightData_.zPosEdit, SLOT(lightZPosSlot(double)));

  connectRealSpin(lightData_.xDirEdit, SLOT(lightXDirSlot(double)));
  connectRealSpin(lightData_.yDirEdit, SLOT(lightYDirSlot(double)));
  connectRealSpin(lightData_.zDirEdit, SLOT(lightZDirSlot(double)));

  connectRealSpin(lightData_.nearEdit, SLOT(lightNearSlot(double)));
  connectRealSpin(lightData_.farEdit , SLOT(lightFarSlot(double)));
  connectRealSpin(lightData_.fovEdit , SLOT(lightFovSlot(double)));

  connectRealSpin(lightData_.shadowBiasEdit , SLOT(shadowBiasSlot(double)));
//connectRealSpin(lightData_.shadowScaleEdit, SLOT(shadowScaleSlot(double)));

  connectRealSpin(lightData_.pointRadiusEdit , SLOT(lightPointRadiusSlot(double)));
  connectRealSpin(lightData_.attenuation0Edit, SLOT(lightConstantAttenuationSlot(double)));
  connectRealSpin(lightData_.attenuation1Edit, SLOT(lightLinearAttenuationSlot(double)));
  connectRealSpin(lightData_.attenuation2Edit, SLOT(lightQuadraticAttenuationSlot(double)));

  connectPointEdit(lightData_.spotDirectionEdit, SLOT(lightSpotDirectionSlot()));

  connectRealSpin(lightData_.spotExponentEdit    , SLOT(lightSpotExponentSlot(double)));
  connectRealSpin(lightData_.spotCutOffAngle     , SLOT(lightCutOffAngleSlot(double)));
  connectRealSpin(lightData_.spotOuterCutOffAngle, SLOT(lightOuterCutOffAngleSlot(double)));

  connectCheckBox(overviewData_.cameraCheck, SLOT(overviewCameraSlot(int)));
  connectCheckBox(overviewData_.lightCheck , SLOT(overviewLightSlot(int)));
}

void
Control::
updateWidgets()
{
  connectSlots(false);

  auto *canvas = app_->canvas();

  //---

  generalData_.skyboxCheck->setChecked(canvas->isSkybox());

  generalData_.shadowMapDebug->setChecked(app_->isShadowMapDebug());

  //---

  auto *camera = this->camera();

  cameraData_.disableRollCheck->setChecked(camera->isDisableRoll());

  cameraData_.clampPitchCheck->setChecked(camera->isClampPitch());
  cameraData_.minPitchEdit   ->setValue(camera->minPitch());
  cameraData_.maxPitchEdit   ->setValue(camera->maxPitch());

  cameraData_.clampYawCheck->setChecked(camera->isClampYaw());
  cameraData_.minYawEdit   ->setValue  (camera->minYaw());
  cameraData_.maxYawEdit   ->setValue  (camera->maxYaw());

  cameraData_.clampRollCheck->setChecked(camera->isClampRoll());
  cameraData_.minRollEdit   ->setValue  (camera->minRoll());
  cameraData_.maxRollEdit   ->setValue  (camera->maxRoll());

  cameraData_.xOriginEdit->setValue(camera->origin().x());
  cameraData_.yOriginEdit->setValue(camera->origin().y());
  cameraData_.zOriginEdit->setValue(camera->origin().z());

  cameraData_.xPosEdit->setValue(camera->position().x());
  cameraData_.yPosEdit->setValue(camera->position().y());
  cameraData_.zPosEdit->setValue(camera->position().z());

  cameraData_.xDirEdit->setValue(camera->front().x());
  cameraData_.yDirEdit->setValue(camera->front().y());
  cameraData_.zDirEdit->setValue(camera->front().z());

  cameraData_.distanceEdit->setValue(camera->distance());

  cameraData_.pitchEdit->setValue(camera->pitch());
  cameraData_.yawEdit  ->setValue(camera->yaw());
  cameraData_.rollEdit ->setValue(camera->roll());

  cameraData_.nearEdit->setValue(camera->near());
  cameraData_.farEdit ->setValue(camera->far());
  cameraData_.fovEdit ->setValue(camera->fov());

  //---

  auto *light = this->light();

  lightData_.typeCombo->setCurrentIndex(lightTypeInd.typeToInd(light->getType()));

  lightData_.xOriginEdit->setValue(light->origin().x());
  lightData_.yOriginEdit->setValue(light->origin().y());
  lightData_.zOriginEdit->setValue(light->origin().z());

  lightData_.xPosEdit->setValue(light->position().x());
  lightData_.yPosEdit->setValue(light->position().y());
  lightData_.zPosEdit->setValue(light->position().z());

  lightData_.xDirEdit->setValue(light->getDirection().x());
  lightData_.yDirEdit->setValue(light->getDirection().y());
  lightData_.zDirEdit->setValue(light->getDirection().z());

  lightData_.nearEdit->setValue(light->near());
  lightData_.farEdit ->setValue(light->far());
  lightData_.fovEdit ->setValue(light->fov());

  lightData_.shadowBiasEdit ->setValue(canvas->shadowBias());
//lightData_.shadowScaleEdit->setValue(canvas->shadowScale());

  /// direction light
  lightData_.directionEdit->setValue(light->getDirection().point());

  // point light
  lightData_.pointRadiusEdit->setValue(light->getPointRadius());

  lightData_.attenuation0Edit->setValue(light->getConstantAttenuation());
  lightData_.attenuation1Edit->setValue(light->getLinearAttenuation());
  lightData_.attenuation2Edit->setValue(light->getQuadraticAttenuation());

  // spot light
  lightData_.spotDirectionEdit   ->setValue(light->getSpotDirection().point());
  lightData_.spotExponentEdit    ->setValue(light->getSpotExponent());
  lightData_.spotCutOffAngle     ->setValue(light->getSpotCutOffAngle());
  lightData_.spotOuterCutOffAngle->setValue(light->getSpotOuterCutOffAngle());

  //---

  auto *overview = app_->overview();

  overviewData_.cameraCheck->setChecked(overview->isShowCamera());
  overviewData_.lightCheck ->setChecked(overview->isShowLight ());

  //---

  connectSlots(true);
}

//---

void
Control::
skyboxVisibleSlot(int state)
{
  auto *canvas = app_->canvas();

  canvas->setSkybox(state);

  canvas->update();
}

void
Control::
shadowMapDebugSlot(int state)
{
  app_->setShadowMapDebug(state);

  app_->canvas()->update();
}

//---

void
Control::
disableRollSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setDisableRoll(state);
}

void
Control::
clampPitchSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setClampPitch(state);
}

void
Control::
minPitchSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMinPitch(r);
}

void
Control::
maxPitchSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMaxPitch(r);
}

void
Control::
clampYawSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setClampYaw(state);
}

void
Control::
minYawSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMinYaw(r);
}

void
Control::
maxYawSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMaxYaw(r);
}

void
Control::
clampRollSlot(int state)
{
  UpdateScope updateScope(this);

  camera()->setClampRoll(state);
}

void
Control::
minRollSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMinRoll(r);
}

void
Control::
maxRollSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setMaxRoll(r);
}

void
Control::
cameraXOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = camera()->origin(); o.setX(r);

  camera()->setOrigin(o);
}

void
Control::
cameraYOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = camera()->origin(); o.setY(r);

  camera()->setOrigin(o);
}

void
Control::
cameraZOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = camera()->origin(); o.setZ(r);

  camera()->setOrigin(o);
}

void
Control::
cameraXPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setX(r);

  camera()->setPosition(p);
}

void
Control::
cameraYPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setY(r);

  camera()->setPosition(p);
}

void
Control::
cameraZPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setZ(r);

  camera()->setPosition(p);
}

void
Control::
cameraXDirSlot(double /*r*/)
{
#if 0
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setX(r);

  camera()->setDirection(p);
#endif
}

void
Control::
cameraYDirSlot(double /*r*/)
{
#if 0
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setY(r);

  camera()->setDirection(p);
#endif
}

void
Control::
cameraZDirSlot(double /*r*/)
{
#if 0
  UpdateScope updateScope(this);

  auto p = camera()->position(); p.setZ(r);

  camera()->setDirection(p);
#endif
}

void
Control::
distanceSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setDistance(r);
}

void
Control::
pitchSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setPitch(r);
}

void
Control::
yawSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setYaw(r);
}

void
Control::
rollSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setRoll(r);
}

void
Control::
cameraNearSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setNear(r);
}

void
Control::
cameraFarSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setFar(r);
}

void
Control::
cameraFovSlot(double r)
{
  UpdateScope updateScope(this);

  camera()->setFov(r);
}

//---

void
Control::
lightTypeSlot(int ind)
{
  light()->setType(lightTypeInd.indToType(ind));

  app_->canvas()->update();
}

void
Control::
lightXOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = light()->origin(); o.setX(r);

  light()->setOrigin(o);
}

void
Control::
lightYOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = light()->origin(); o.setY(r);

  light()->setOrigin(o);
}

void
Control::
lightZOriginSlot(double r)
{
  UpdateScope updateScope(this);

  auto o = light()->origin(); o.setZ(r);

  light()->setOrigin(o);
}

void
Control::
lightXPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = light()->position(); p.setX(r);

  light()->setPosition(p.point());
}

void
Control::
lightYPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = light()->position(); p.setY(r);

  light()->setPosition(p.point());
}

void
Control::
lightZPosSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = light()->position(); p.setZ(r);

  light()->setPosition(p.point());
}

void
Control::
lightXDirSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = light()->getDirection(); p.setX(r);

  light()->setDirection(p);
}

void
Control::
lightYDirSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = light()->getDirection(); p.setY(r);

  light()->setDirection(p);
}

void
Control::
lightZDirSlot(double r)
{
  UpdateScope updateScope(this);

  auto p = light()->getDirection(); p.setZ(r);

  light()->setDirection(p);
}

void
Control::
lightNearSlot(double r)
{
  UpdateScope updateScope(this);

  light()->setNear(r);
}

void
Control::
lightFarSlot(double r)
{
  UpdateScope updateScope(this);

  light()->setFar(r);
}

void
Control::
lightFovSlot(double r)
{
  UpdateScope updateScope(this);

  light()->setFov(r);
}

#if 0
void
Control::
shadowScaleSlot(double r)
{
  UpdateScope updateScope(this);

  app_->canvas()->setShadowScale(r);
}
#endif

void
Control::
shadowBiasSlot(double r)
{
  UpdateScope updateScope(this);

  app_->canvas()->setShadowBias(r);
}

void
Control::
lightPointRadiusSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  light->setPointRadius(r);

  canvas->update();
}

void
Control::
lightConstantAttenuationSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  light->setConstantAttenuation(r);

  canvas->update();
}

void
Control::
lightLinearAttenuationSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  light->setLinearAttenuation(r);

  canvas->update();
}

void
Control::
lightQuadraticAttenuationSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  light->setQuadraticAttenuation(r);

  canvas->update();
}

void
Control::
lightSpotDirectionSlot()
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  auto p = lightData_.spotDirectionEdit->getValue();

  light->setSpotDirection(CVector3D(p));

  canvas->update();
}

void
Control::
lightSpotExponentSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  light->setSpotExponent(r);

  canvas->update();
}

void
Control::
lightCutOffAngleSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  light->setSpotCutOffAngle(r);

  canvas->update();
}

void
Control::
lightOuterCutOffAngleSlot(double r)
{
  auto *canvas = app_->canvas();
  auto *light  = canvas->light();

  light->setSpotOuterCutOffAngle(r);

  canvas->update();
}

void
Control::
syncSlot()
{
  auto *camera = this->camera();
  auto *light  = this->light();

  light->setPosition(camera->position().point());

  light->setPitch(camera->pitch());
  light->setYaw  (camera->yaw());
  light->setRoll (camera->roll());
}

void
Control::
overviewCameraSlot(int state)
{
  auto *overview = app_->overview();

  overview->setShowCamera(state);

  overview->update();
}

void
Control::
overviewLightSlot(int state)
{
  auto *overview = app_->overview();

  overview->setShowLight(state);

  overview->update();
}

//---

Camera *
Control::
camera() const
{
  return app_->canvas()->camera();
}

Light *
Control::
light() const
{
  return app_->canvas()->light();
}

void
Control::
doUpdate()
{
  updateWidgets();

  app_->canvas()->update();
}

}
