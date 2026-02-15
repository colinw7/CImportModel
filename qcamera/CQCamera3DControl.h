#ifndef CQCamera3DControl_H
#define CQCamera3DControl_H

#include <CQCamera3DApp.h>

#include <QFrame>

class CGLCameraIFace;
class CQCamera3DObjectsList;
class CQCamera3DLightList;
class CQCamera3DCameraList;
class CQCamera3DTextureChooser;
class CQCamera3DTextureList;
class CQCamera3DMaterialChooser;
class CQCamera3DMaterialList;
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

class CQCamera3DControl : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DControl(CQCamera3DApp *app);

  void connectSlots(bool b);

 private Q_SLOTS:
  void viewTypeSlot();

  void updateWidgets();
  void updateTextureWidgets(bool disconnect=true);
  void updateCameraWidgets (bool disconnect=true);

  void mainTabSlot(int);

  // General
  void showWireframeSlot(int);
  void showSolidSlot(int);
  void showTexturedSlot(int);
  void showPointsSlot(int i);

  void wireframeColorSlot(const QColor &c);

  void quadViewSlot(int);
  void debugCameraSlot(int);
  void pointSizeSlot(double);
  void lineWidthSlot(double);
  void cursorSlot();

  void normalsPointsSlot(int);
  void normalsFacesSlot(int);
  void normalsSizeSlot(double r);
  void normalsColorSlot(const QColor &c);

  void showBasisSlot(int);

  void showBBoxSlot(int);
  void bboxOrientSlot(int);

  void depthTestSlot(int);
  void cullSlot(int);
  void frontFaceSlot(int);

  // Camera
  void showCameraSlot(int);
  void showEyeLineSlot(int);
  void showPlanesSlot(int);

  void currentCameraSlot();

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

  void aroundDeltaSlot(double d);

  void resetCameraSlot();
  void topCameraSlot();
  void sideCameraSlot();
  void frontCameraSlot();

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
  void showXAxesSlot(int);
  void showYAxesSlot(int);
  void showZAxesSlot(int);

  void axesXPosSlot(double);
  void axesYPosSlot(double);
  void axesZPosSlot(double);

  void axesGridSlot(int);

  // Mouse
#if 0
  void mouseTypeSlot(int);
  void editTypeSlot(int);
#endif
  void mouseScaleSlot(double);
  void mouseBasisSlot(int);

  // Selection
//void selectTypeSlot(int);

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
  void connectMaterialEdit();

  void swapSlot();
  void invertSlot();

  void addTextureSlot();
  void loadTextureMapSlot();
  void saveTextureMapSlot();

  void loadMaterialMapSlot();
  void saveMaterialMapSlot();
  void addMaterialSlot();

  void selectParentSlot();
  void selectFacesSlot();
  void selectPointsSlot();
  void selectRelatedSlot();
  void deselectSlot();
  void selectDumpSlot();

  // Objects
  void objectSelectSlot();
  void objectZoomSlot();

  void addConeSlot();
  void addCubeSlot();
  void addCylinderSlot();
  void addPyramidSlot();
  void addSphereSlot();
  void addTorusSlot();

  void loadModelSlot();
  void saveModelSlot();

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

  // Objects
  void currentObjectChangedSlot();
  void objectAutoSelectSlot(int i);

  // Overview
  void overviewEqualScaleSlot(int i);
//void overviewSelectTypeSlot(int i);
//void overviewMouseTypeSlot(int i);
  void overviewModelTypeSlot(int i);
  void overviewCameraSlot(int i);
  void overviewLightsSlot(int i);

  // UV Map
  void uvTextureTypeSlot(int i);
  void uvTextureWrapSlot(int i);

  // Textures
//void textureFlipSlot(int i);
  void textureWrapSlot(int i);

  // Bones
  void bonesModelSlot(int);
  void bonesBoneNodeSlot(int i);
  void bonesPointJointsSlot(int i);
  void bonesOnlyJointsSlot(int i);

  void bonesTranslationSlot();
  void bonesRotationSlot();
  void bonesScaleSlot();

  void currentBoneSlot();
  void updateBones();
  void updateCurrentBone();
  void updateAnim();

  // Animation
  void timerSlot();

  void animNameSlot();
  void animTimeSlot(double t);
  void animTimeStepSlot(double t);

  void playSlot();
  void pauseSlot();
  void stepSlot();

  void animStep();

 private:
  void updateObjects();

 private:
  CGLCameraIFace *getCamera() const;

 private:
  struct PlusMinusData {
    QToolButton* minusButton { nullptr };
    QToolButton* plusButton  { nullptr };
  };

  CQCamera3DApp* app_ { nullptr };

  QTabWidget *mainTab_ { nullptr };

  CQCamera3DViewType viewType_ { CQCamera3DViewType::NONE };

  // General
  struct GeneralData {
    QCheckBox* showWireframeCheck { nullptr };
    QCheckBox* showSolidCheck     { nullptr };
    QCheckBox* showTexturedCheck  { nullptr };
    QCheckBox* showPointsCheck    { nullptr };

    CQColorEdit* wireframeColorEdit { nullptr };

    QCheckBox* quadViewCheck    { nullptr };
    QCheckBox* debugCameraCheck { nullptr };

    CQRealSpin*    pointSizeEdit { nullptr };
    CQRealSpin*    lineWidthEdit { nullptr };
    CQPoint3DEdit* cursorEdit    { nullptr };

    QCheckBox*   normalsPointsCheck { nullptr };
    QCheckBox*   normalsFacesCheck  { nullptr };
    CQRealSpin*  normalsSizeEdit    { nullptr };
    CQColorEdit* normalsColorEdit   { nullptr };

    QCheckBox* showBasisCheck { nullptr };

    QCheckBox* showBBoxCheck   { nullptr };
    QCheckBox* bboxOrientCheck { nullptr };

    QCheckBox* depthTestCheck { nullptr };
    QCheckBox* cullFaceCheck  { nullptr };
    QCheckBox* frontFaceCheck { nullptr };
  };

  GeneralData generalData_;

  // Camera
  struct CameraData {
    QCheckBox* showCheck    { nullptr };
    QCheckBox* eyeLineCheck { nullptr };
    QCheckBox* planesCheck  { nullptr };

    CQCamera3DCameraList* cameraList { nullptr };

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
    CQRealSpin*   aroundDeltaEdit { nullptr };

    double aroundDelta { -1 };
  };

  CameraData cameraData_;

  // Lights
  struct LightsData {
    CQCamera3DLightList *lightList { nullptr };

    CQColorEdit*   ambientColorEdit     { nullptr };
    CQRealSpin*    ambientStrengthEdit  { nullptr };
    CQRealSpin*    diffuseStrengthEdit  { nullptr };
    CQRealSpin*    emissiveStrengthEdit { nullptr };
    CQRealSpin*    specularStrengthEdit { nullptr };
    QCheckBox*     fixedDiffuseCheck    { nullptr };
    QComboBox*     typeCombo            { nullptr };
#if 0
    QCheckBox*     enabledCheck         { nullptr };
#endif
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
    using CheckEdit = std::pair<QCheckBox *, CQRealSpin *>;

    CheckEdit xPosEdit;
    CheckEdit yPosEdit;
    CheckEdit zPosEdit;

    QCheckBox* gridCheck { nullptr };
  };

  AxisData axisData_;

  // Mouse
#if 0
  QComboBox*  mouseTypeCombo_  { nullptr };
  QComboBox*  editTypeCombo_   { nullptr };
#endif
  CQRealSpin* mouseScaleEdit_  { nullptr };
  QCheckBox*  mouseBasisCheck_ { nullptr };

  // Selection
  struct SelectionData {
    QComboBox*   typeCombo { nullptr };
    CQTextLabel* indLabel  { nullptr };
    QCheckBox*   visCheck  { nullptr };

    CQPoint3DEdit* centerEdit     { nullptr };
    CQPoint3DEdit* sizeEdit       { nullptr };
    CQMatrix3D*    meshMatrixEdit { nullptr };

    CQPoint3DEdit* translationEdit { nullptr };
    CQPoint3DEdit* rotationEdit    { nullptr };
    CQPoint3DEdit* scaleEdit       { nullptr };
    CQMatrix3D*    matrixEdit      { nullptr };

    CQColorEdit* colorEdit { nullptr };

    CQCamera3DTextureChooser *diffuseTextureEdit  { nullptr };
    CQCamera3DTextureChooser *normalTextureEdit   { nullptr };
    CQCamera3DTextureChooser *specularTextureEdit { nullptr };
    CQCamera3DTextureChooser *emissiveTextureEdit { nullptr };

    CQCamera3DMaterialChooser *materialNameChooser { nullptr };

    QTextEdit* objectInfoText { nullptr };
  };

  SelectionData selectionData_;

  // Objects
  struct ObjectsData {
    CQCamera3DObjectsList* objectsList     { nullptr };
    QCheckBox*             autoSelectCheck { nullptr };

    bool autoSelect { false };
  };

  ObjectsData objectsData_;

  // Textures
  struct TexturesData {
    CQCamera3DTextureList* textureList { nullptr };
//  QCheckBox*             flipCheck   { nullptr };
    QCheckBox*             wrapCheck   { nullptr };
  };

  TexturesData texturesData_;

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
//  QComboBox* mouseTypeCombo  { nullptr };
//  QComboBox* selectTypeCombo { nullptr };
    QComboBox* modelTypeCombo  { nullptr };
    QCheckBox* cameraCheck     { nullptr };
    QCheckBox* lightsCheck     { nullptr };
  };

  OverviewData overviewData_;

  // UV
  struct UVData {
    QComboBox *typeCombo { nullptr };
    QCheckBox *wrapCheck { nullptr };
  };

  UVData uvData_;

  // Bones
  struct BonesData {
    QCheckBox* modelCheck       { nullptr };
    QCheckBox* boneNodesCheck   { nullptr };
    QCheckBox* pointJointsCheck { nullptr };
    QCheckBox* onlyJointsCheck  { nullptr };

    CQTextLabel* nodeLabel     { nullptr };
    CQTextLabel* childrenLabel { nullptr };
    QCheckBox*   jointCheck    { nullptr };
    CQTextLabel* objectLabel   { nullptr };

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
    CQCamera3DAnimChooser* animCombo    { nullptr };
    CQRealSpin*            timeEdit     { nullptr };
    CQRealSpin*            timeStepEdit { nullptr };
    CQIconButton*          playButton   { nullptr };
    CQIconButton*          pauseButton  { nullptr };
    CQIconButton*          stepButton   { nullptr };
    CQTextLabel*           nodeLabel    { nullptr };

    QComboBox*    translationInterpCombo { nullptr };
    QTableWidget* animTranslationTable   { nullptr };

    QComboBox*    rotationInterpCombo { nullptr };
    QTableWidget* animRotationTable   { nullptr };

    QComboBox*    scaleInterpCombo { nullptr };
    QTableWidget* animScaleTable   { nullptr };

    bool running { false };
  };

  AnimData animData_;

  bool connected_ { false };
};

#endif
