#ifndef CQCamera3DControl_H
#define CQCamera3DControl_H

#include <QFrame>

class CQCamera3DApp;
class CQCamera3DCamera;
class CQCamera3DTextureChooser;

#if 0
class CQMatrix3D;
#else
class CQPoint3DEdit;
#endif
class CQRealSpin;
class CQColorEdit;

class QComboBox;
class QCheckBox;
class QToolButton;
class QLabel;

class CQCamera3DControl : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DControl(CQCamera3DApp *app);

  void connectSlots(bool b);

  void updateWidgets();

 private Q_SLOTS:
  void showWireframeSlot(int);
  void showSolidSlot(int);

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

  // Axis
  void showAxesSlot(int);

  void axesXPosSlot(double);
  void axesYPosSlot(double);
  void axesZPosSlot(double);

  // Mouse
  void mouseTypeSlot(int);

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

  void diffuseMapSlot();
  void normalMapSlot();
  void specularMapSlot();
  void emissiveMapSlot();

  void addTextureSlot();
  void selectParentSlot();
  void deselectSlot();

  // Objects
  void addCubeSlot();
  void addCylinderSlot();
  void addPyramidSlot();
  void addSphereSlot();
  void addTorusSlot();

  void addModelSlot();

  // Overview
  void overviewEqualScaleSlot(int i);

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

  // General
  QCheckBox* showWireframeCheck_ { nullptr };
  QCheckBox* showSolidCheck_     { nullptr };

  QCheckBox* depthTestCheck_ { nullptr };
  QCheckBox* cullFaceCheck_  { nullptr };
  QCheckBox* frontFaceCheck_ { nullptr };

  // Camera
  QCheckBox* altCheck_ { nullptr };

  QCheckBox* showCameraCheck_  { nullptr };
  QCheckBox* showEyeLineCheck_ { nullptr };
  QCheckBox* showPlanesCheck_  { nullptr };

  CQRealSpin* pitchEdit_ { nullptr };
  CQRealSpin* yawEdit_   { nullptr };
  CQRealSpin* rollEdit_  { nullptr };

  CQRealSpin* nearEdit_ { nullptr };
  CQRealSpin* farEdit_  { nullptr };

  CQRealSpin* xOriginEdit_ { nullptr };
  CQRealSpin* yOriginEdit_ { nullptr };
  CQRealSpin* zOriginEdit_ { nullptr };

  CQRealSpin* xEdit_ { nullptr };
  CQRealSpin* yEdit_ { nullptr };
  CQRealSpin* zEdit_ { nullptr };

  CQRealSpin* fovEdit_   { nullptr };
  CQRealSpin* eyeZ1Edit_ { nullptr };
  CQRealSpin* eyeZ2Edit_ { nullptr };

  PlusMinusData aroundXButtons_;
  PlusMinusData aroundYButtons_;
  PlusMinusData aroundZButtons_;

  // Axis
  QCheckBox* showAxesCheck_ { nullptr };

  CQRealSpin* axesXPos_ { nullptr };
  CQRealSpin* axesYPos_ { nullptr };
  CQRealSpin* axesZPos_ { nullptr };

  // Mouse
  QComboBox* mouseTypeCombo_ { nullptr };

  // Selection
  QComboBox* selectTypeCombo_ { nullptr };
  QLabel*    objectIndLabel_  { nullptr };
  QCheckBox* objectVisCheck_  { nullptr };

#if 0
  CQMatrix3D*    objectMatrix_    { nullptr };
#else
  CQPoint3DEdit* translationEdit_ { nullptr };
  CQPoint3DEdit* rotationEdit_    { nullptr };
  CQPoint3DEdit* scaleEdit_       { nullptr };
#endif

  CQColorEdit* objectColor_ { nullptr };

  CQCamera3DTextureChooser *diffuseTextureEdit_  { nullptr };
  CQCamera3DTextureChooser *specularTextureEdit_ { nullptr };
  CQCamera3DTextureChooser *normalTextureEdit_   { nullptr };
  CQCamera3DTextureChooser *emissiveTextureEdit_ { nullptr };

  // Overview
  QCheckBox* overviewEqualScale_ { nullptr };
};

#endif
