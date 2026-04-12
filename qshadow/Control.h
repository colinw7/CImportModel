#ifndef Control_H
#define Control_H

#include <QFrame>

class CQRealSpin;
class CQPoint3DEdit;

class QCheckBox;
class QComboBox;

namespace CQShadow3D {

class App;
class Camera;
class Light;

class Control : public QFrame {
  Q_OBJECT

 public:
  Control(App *app);

 private:
  struct UpdateScope {
    UpdateScope(Control *control) : control_(control) { control_->connectSlots(false); }

   ~UpdateScope() { control_->connectSlots(true); control_->doUpdate(); }

    Control* control_ { nullptr };
  };

  friend class UpdateScope;

  void connectSlots(bool);

  Camera *camera() const;

  Light *light() const;

  void doUpdate();

 private Q_SLOTS:
  void updateWidgets();

  void skyboxVisibleSlot(int);
  void shadowMapDebugSlot(int);

  void disableRollSlot(int);

  void clampPitchSlot(int);
  void minPitchSlot(double);
  void maxPitchSlot(double);

  void clampYawSlot(int);
  void minYawSlot(double);
  void maxYawSlot(double);

  void clampRollSlot(int);
  void minRollSlot(double);
  void maxRollSlot(double);

  void cameraXOriginSlot(double);
  void cameraYOriginSlot(double);
  void cameraZOriginSlot(double);

  void cameraXPosSlot(double);
  void cameraYPosSlot(double);
  void cameraZPosSlot(double);

  void cameraXDirSlot(double);
  void cameraYDirSlot(double);
  void cameraZDirSlot(double);

  void distanceSlot(double);

  void pitchSlot(double);
  void yawSlot(double);
  void rollSlot(double);

  void cameraNearSlot(double);
  void cameraFarSlot(double);
  void cameraFovSlot(double);

  //---

  void lightTypeSlot(int);

  void lightXOriginSlot(double);
  void lightYOriginSlot(double);
  void lightZOriginSlot(double);

  void lightXPosSlot(double);
  void lightYPosSlot(double);
  void lightZPosSlot(double);

  void lightXDirSlot(double);
  void lightYDirSlot(double);
  void lightZDirSlot(double);

  void lightNearSlot(double);
  void lightFarSlot(double);
  void lightFovSlot(double);

//void shadowScaleSlot(double);
  void shadowBiasSlot(double);

  void lightPointRadiusSlot(double);

  void lightConstantAttenuationSlot(double);
  void lightLinearAttenuationSlot(double);
  void lightQuadraticAttenuationSlot(double);

  void lightSpotDirectionSlot();
  void lightSpotExponentSlot(double);
  void lightCutOffAngleSlot(double);
  void lightOuterCutOffAngleSlot(double);

  //---

  void overviewCameraSlot(int);
  void overviewLightSlot(int);

  //---

  void syncSlot();

 private:
  App* app_ { nullptr };

  struct GeneralData {
    QCheckBox* skyboxCheck    { nullptr };
    QCheckBox* shadowMapDebug { nullptr };
  };

  struct CameraData {
    QCheckBox* disableRollCheck { nullptr };

    QCheckBox*  clampPitchCheck { nullptr };
    CQRealSpin* minPitchEdit    { nullptr };
    CQRealSpin* maxPitchEdit    { nullptr };

    QCheckBox*  clampYawCheck { nullptr };
    CQRealSpin* minYawEdit    { nullptr };
    CQRealSpin* maxYawEdit    { nullptr };

    QCheckBox*  clampRollCheck { nullptr };
    CQRealSpin* minRollEdit    { nullptr };
    CQRealSpin* maxRollEdit    { nullptr };

    CQRealSpin* xOriginEdit { nullptr };
    CQRealSpin* yOriginEdit { nullptr };
    CQRealSpin* zOriginEdit { nullptr };

    CQRealSpin* xPosEdit { nullptr };
    CQRealSpin* yPosEdit { nullptr };
    CQRealSpin* zPosEdit { nullptr };

    CQRealSpin* xDirEdit { nullptr };
    CQRealSpin* yDirEdit { nullptr };
    CQRealSpin* zDirEdit { nullptr };

    CQRealSpin* distanceEdit { nullptr };

    CQRealSpin* pitchEdit { nullptr };
    CQRealSpin* yawEdit   { nullptr };
    CQRealSpin* rollEdit  { nullptr };

    CQRealSpin* nearEdit { nullptr };
    CQRealSpin* farEdit  { nullptr };
    CQRealSpin* fovEdit  { nullptr };
  };

  struct LightData {
    QComboBox *typeCombo { nullptr };

    CQRealSpin* xOriginEdit { nullptr };
    CQRealSpin* yOriginEdit { nullptr };
    CQRealSpin* zOriginEdit { nullptr };

    CQRealSpin* xPosEdit { nullptr };
    CQRealSpin* yPosEdit { nullptr };
    CQRealSpin* zPosEdit { nullptr };

    CQRealSpin* xDirEdit { nullptr };
    CQRealSpin* yDirEdit { nullptr };
    CQRealSpin* zDirEdit { nullptr };

    CQRealSpin* nearEdit { nullptr };
    CQRealSpin* farEdit  { nullptr };
    CQRealSpin* fovEdit  { nullptr };

//  CQRealSpin* shadowScaleEdit { nullptr };
    CQRealSpin* shadowBiasEdit  { nullptr };

    CQPoint3DEdit* directionEdit { nullptr };

    CQRealSpin* pointRadiusEdit { nullptr };

    CQRealSpin* attenuation0Edit { nullptr };
    CQRealSpin* attenuation1Edit { nullptr };
    CQRealSpin* attenuation2Edit { nullptr };

    CQPoint3DEdit* spotDirectionEdit { nullptr };
    CQRealSpin*    spotExponentEdit  { nullptr };

    CQRealSpin* spotCutOffAngle      { nullptr };
    CQRealSpin* spotOuterCutOffAngle { nullptr };
  };

  struct OverviewData {
    QCheckBox *cameraCheck { nullptr };
    QCheckBox *lightCheck  { nullptr };
  };

  GeneralData  generalData_;
  CameraData   cameraData_;
  LightData    lightData_;
  OverviewData overviewData_;
};

}

#endif
