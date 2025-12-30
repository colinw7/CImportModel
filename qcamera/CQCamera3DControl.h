#ifndef CQCamera3DControl_H
#define CQCamera3DControl_H

#include <QFrame>

class CQCamera3DApp;
class CQCamera3DCamera;
class CQCamera3DObjectsList;
class CQCamera3DMaterialList;
class CQCamera3DLightList;
class CQCamera3DTextureChooser;
class CQCamera3DAnimChooser;
class CQCamera3DBonesList;

class CQMatrix3D;
class CQPoint3DEdit;
class CQPoint4DEdit;
class CQRealSpin;
class CQColorEdit;
class CQTextLabel;
class CQIconButton;

class QTabWidget;
class QTableWidget;
class QTextEdit;
class QComboBox;
class QCheckBox;
class QToolButton;
class QLineEdit;
class QLabel;

class CQCamera3DControl : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DControl(CQCamera3DApp *app);

  void connectSlots(bool b);

  void setCurrentControl(int i);

 private Q_SLOTS:
  void updateWidgets();

  void mainTabSlot(int);

  void showWireframeSlot(int);
  void showSolidSlot(int);
  void showPointsSlot(int i);

  void pointSizeSlot(double);
  void lineWidthSlot(double);

  void showNormalsSlot(int);
  void normalsSizeSlot(double r);
  void normalsColorSlot(const QColor &c);

  void depthTestSlot(int);
  void cullSlot(int);
  void frontFaceSlot(int);

  // Camera
  void altSlot(int);

  void showCameraSlot(int);
  void showEyeLineSlot(int);
  void showPlanesSlot(int);

  void pitchSlot(double r);
  void yawSlot(double r);
  void rollSlot(double r);

  void nearSlot(double r);
  void farSlot (double r);

  void xOriginSlot(double r);
  void yOriginSlot(double r);
  void zOriginSlot(double r);

  void xSlot(double r);
  void ySlot(double r);
  void zSlot(double r);

  void fovSlot(double r);
  void eyeZ1Slot(double r);
  void eyeZ2Slot(double r);

  void aroundXSlot1();
  void aroundXSlot2();
  void aroundYSlot1();
  void aroundYSlot2();
  void aroundZSlot1();
  void aroundZSlot2();

  void resetSlot();
  void topSlot();
  void sideSlot();
  void frontSlot();

  // Lights
  void currentLightSlot();

  void ambientColorSlot(const QColor &);
  void ambientStrengthSlot(double);
  void diffuseStrengthSlot(double r);
  void emissiveStrengthSlot(double r);
  void specularStrengthSlot(double r);
  void fixedDiffuseSlot(int i);

  void lightTypeSlot(int);
  void lightEnabledSlot(int);
  void lightPositionSlot();
  void lightColorSlot(const QColor &);
  void lightDirectionSlot();
  void lightPointRadiusSlot(double);
  void lightConstantAttenuationSlot(double);
  void lightLinearAttenuationSlot(double);
  void lightQuadraticAttenuationSlot(double);
  void lightSpotDirectionSlot();
  void lightSpotExponentSlot(double);
  void lightCutOffAngleSlot(double);
  void addLightSlot();
  void resetLightSlot();

  // Axis
  void showAxesSlot(int);

  void axesXPosSlot(double);
  void axesYPosSlot(double);
  void axesZPosSlot(double);

  // Mouse
  void mouseTypeSlot(int);
  void mouseScaleSlot(double);

  // Selection
  void selectTypeSlot(int);

  void objectVisSlot(int);

#if 0
  void objectMatrixSlot();
#else
  void translationSlot();
  void rotationSlot();
  void scaleSlot();
#endif

  void objectColorSlot(const QColor &);

  void diffuseTextureSlot();
  void normalTextureSlot();
  void specularTextureSlot();
  void emissiveTextureSlot();

  void swapSlot();
  void invertSlot();

  void addTextureSlot();
  void loadTextureMapSlot();
  void saveTextureMapSlot();

  void loadMaterialMapSlot();
  void saveMaterialMapSlot();

  void selectParentSlot();
  void selectRelatedSlot();
  void deselectSlot();

  // Objects
  void objectSelectSlot();
  void objectZoomSlot();

  void addCubeSlot();
  void addCylinderSlot();
  void addPyramidSlot();
  void addSphereSlot();
  void addTorusSlot();

  void addModelSlot();

  // Material
  void currentMaterialSlot();

  void materialAmbientSlot(const QColor &c);
  void materialDiffuseSlot(const QColor &c);
  void materialSpecularSlot(const QColor &c);
  void materialEmissionSlot(const QColor &c);

  void materialShininessSlot(double r);
  void materialTransparencySlot(double r);

  void materialAmbientTextureSlot();
  void materialDiffuseTextureSlot();
  void materialNormalTextureSlot();
  void materialSpecularTextureSlot();
  void materialEmissiveTextureSlot();

  // Overview
  void overviewEqualScaleSlot(int i);
  void overviewSelectTypeSlot(int i);
  void overviewMouseTypeSlot(int i);
  void overviewModelTypeSlot(int i);
  void overviewCameraSlot(int i);
  void overviewLightsSlot(int i);

  // UV Map
  void uvTextureTypeSlot(int i);

  // Bones
  void bonesModelSlot(int);
  void bonesBoneNodeSlot(int i);
  void bonesPointJointsSlot(int i);
  void currentBoneSlot();
  void updateBones();
  void updateAnim();

  // Animation
  void timerSlot();

  void animNameSlot();
  void animTimeSlot(double t);

  void playSlot();
  void pauseSlot();
  void stepSlot();

  void animStep();

 private:
  void updateObjects();

 private:
  CQCamera3DCamera *getCamera() const;

 private:
  struct PlusMinusData {
    QToolButton* minusButton { nullptr };
    QToolButton* plusButton  { nullptr };
  };

  CQCamera3DApp* app_ { nullptr };

  QTabWidget *mainTab_ { nullptr };

  // General
  QCheckBox* showWireframeCheck_ { nullptr };
  QCheckBox* showSolidCheck_     { nullptr };
  QCheckBox* showPointsCheck_    { nullptr };

  CQRealSpin* pointSizeEdit_ { nullptr };
  CQRealSpin* lineWidthEdit_ { nullptr };

  QCheckBox*   showNormalsCheck_ { nullptr };
  CQRealSpin*  normalsSizeEdit_  { nullptr };
  CQColorEdit* normalsColorEdit_ { nullptr };

  QCheckBox* depthTestCheck_ { nullptr };
  QCheckBox* cullFaceCheck_  { nullptr };
  QCheckBox* frontFaceCheck_ { nullptr };

  // Camera
  struct CameraData {
    QCheckBox* altCheck { nullptr };

    QCheckBox* showCheck    { nullptr };
    QCheckBox* eyeLineCheck { nullptr };
    QCheckBox* planesCheck  { nullptr };

    CQRealSpin* pitchEdit { nullptr };
    CQRealSpin* yawEdit   { nullptr };
    CQRealSpin* rollEdit  { nullptr };

    CQRealSpin* nearEdit { nullptr };
    CQRealSpin* farEdit  { nullptr };

    CQRealSpin* xOriginEdit { nullptr };
    CQRealSpin* yOriginEdit { nullptr };
    CQRealSpin* zOriginEdit { nullptr };

    CQRealSpin* xEdit { nullptr };
    CQRealSpin* yEdit { nullptr };
    CQRealSpin* zEdit { nullptr };

    CQRealSpin* fovEdit   { nullptr };
    CQRealSpin* eyeZ1Edit { nullptr };
    CQRealSpin* eyeZ2Edit { nullptr };

    PlusMinusData aroundXButtons;
    PlusMinusData aroundYButtons;
    PlusMinusData aroundZButtons;
  };

  CameraData cameraData_;

  // Lights
  struct LightsData {
    CQCamera3DLightList *lightsList { nullptr };

    CQColorEdit*   ambientColorEdit     { nullptr };
    CQRealSpin*    ambientStrengthEdit  { nullptr };
    CQRealSpin*    diffuseStrengthEdit  { nullptr };
    CQRealSpin*    emissiveStrengthEdit { nullptr };
    CQRealSpin*    specularStrengthEdit { nullptr };
    QCheckBox*     fixedDiffuseCheck    { nullptr };
    QComboBox*     typeCombo            { nullptr };
    QCheckBox*     enabledCheck         { nullptr };
    CQPoint3DEdit* directionEdit        { nullptr };
    CQPoint3DEdit* positionEdit         { nullptr };
    CQColorEdit*   colorEdit            { nullptr };
    CQRealSpin*    pointRadiusEdit      { nullptr };
    CQRealSpin*    attenuation0Edit     { nullptr };
    CQRealSpin*    attenuation1Edit     { nullptr };
    CQRealSpin*    attenuation2Edit     { nullptr };
    CQPoint3DEdit* spotDirectionEdit    { nullptr };
    CQRealSpin*    spotExponentEdit     { nullptr };
    CQRealSpin*    spotCutOffAngle      { nullptr };
  };

  LightsData lightsData_;

  // Axis
  struct AxisData {
    QCheckBox* showCheck { nullptr };

    CQRealSpin* xPosEdit { nullptr };
    CQRealSpin* yPosEdit { nullptr };
    CQRealSpin* zPosEdit { nullptr };
  };

  AxisData axisData_;

  // Mouse
  QComboBox*  mouseTypeCombo_ { nullptr };
  CQRealSpin* mouseScaleEdit_ { nullptr };

  // Selection
  struct SelectionData {
    QComboBox*   typeCombo { nullptr };
    CQTextLabel* indLabel  { nullptr };
    QCheckBox*   visCheck  { nullptr };

    CQMatrix3D*    matrixEdit      { nullptr };
    CQPoint3DEdit* translationEdit { nullptr };
    CQPoint3DEdit* rotationEdit    { nullptr };
    CQPoint3DEdit* scaleEdit       { nullptr };

    CQColorEdit* colorEdit { nullptr };

    CQCamera3DTextureChooser *diffuseTextureEdit  { nullptr };
    CQCamera3DTextureChooser *normalTextureEdit   { nullptr };
    CQCamera3DTextureChooser *specularTextureEdit { nullptr };
    CQCamera3DTextureChooser *emissiveTextureEdit { nullptr };

    QLineEdit *materialNameEdit { nullptr };

    QTextEdit* objectInfoText { nullptr };
  };

  SelectionData selectionData_;

  // Objects
  CQCamera3DObjectsList *objectsList_ { nullptr };

  // Materials
  struct MaterialsData {
    CQCamera3DMaterialList *materialList { nullptr };

    CQColorEdit* ambientEdit  { nullptr };
    CQColorEdit* diffuseEdit  { nullptr };
    CQColorEdit* specularEdit { nullptr };
    CQColorEdit* emissionEdit { nullptr };

    CQRealSpin* shininessEdit    { nullptr };
    CQRealSpin* transparencyEdit { nullptr };

    QCheckBox* twoSidedCheck { nullptr };
    QComboBox* shadingCombo  { nullptr };

    CQCamera3DTextureChooser *ambientTextureEdit  { nullptr };
    CQCamera3DTextureChooser *diffuseTextureEdit  { nullptr };
    CQCamera3DTextureChooser *normalTextureEdit   { nullptr };
    CQCamera3DTextureChooser *specularTextureEdit { nullptr };
    CQCamera3DTextureChooser *emissiveTextureEdit { nullptr };
  };

  MaterialsData materialsData_;

  // Overview
  struct OverviewData {
    QCheckBox* equalScale      { nullptr };
    QComboBox* mouseTypeCombo  { nullptr };
    QComboBox* selectTypeCombo { nullptr };
    QComboBox* modelTypeCombo  { nullptr };
    QCheckBox* cameraCheck     { nullptr };
    QCheckBox* lightsCheck     { nullptr };
  };

  OverviewData overviewData_;

  // UV
  struct UVData {
    QComboBox *typeCombo { nullptr };
  };

  UVData uvData_;

  // Bones
  struct BonesData {
    QCheckBox* modelCheck     { nullptr };
    QCheckBox* boneNodesCheck { nullptr };
    QCheckBox* pointJoints    { nullptr };

    QCheckBox* jointCheck { nullptr };

    CQCamera3DBonesList* bonesList { nullptr };

    CQPoint3DEdit* translationEdit { nullptr };
    CQPoint4DEdit* rotationEdit    { nullptr };
    CQPoint3DEdit* scaleEdit       { nullptr };

    CQPoint3DEdit* animTranslationEdit { nullptr };
    CQPoint4DEdit* animRotationEdit    { nullptr };
    CQPoint3DEdit* animScaleEdit       { nullptr };
  };

  BonesData bonesData_;

  // Animation
  struct AnimData {
    CQCamera3DAnimChooser* animCombo   { nullptr };
    CQRealSpin*            timeEdit    { nullptr };
    CQIconButton*          playButton  { nullptr };
    CQIconButton*          pauseButton { nullptr };
    CQIconButton*          stepButton  { nullptr };
    CQTextLabel*           nodeLabel   { nullptr };

    QComboBox*    translationInterpCombo { nullptr };
    QTableWidget* animTranslationTable   { nullptr };

    QComboBox*    rotationInterpCombo { nullptr };
    QTableWidget* animRotationTable   { nullptr };

    QComboBox*    scaleInterpCombo { nullptr };
    QTableWidget* animScaleTable   { nullptr };

    bool running { false };
  };

  AnimData animData_;
};

#endif
