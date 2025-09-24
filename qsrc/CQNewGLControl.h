#ifndef CQNewGLControl_H
#define CQNewGLControl_H

#include <CQNewGLShape.h>

#include <CMatrix3D.h>

#include <QFrame>
#include <QComboBox>
#include <QListWidget>
#include <QTreeWidget>

class CQNewGLModel;
class CQNewGLGeneralControl;
class CQNewGLCameraControl;
class CQNewGLLightsControl;
class CQNewGLNormalsControl;
class CQNewGLAxesControl;
class CQNewGLBBoxControl;
class CQNewGLHullControl;
class CQNewGLBasisControl;
class CQNewGLObjectsControl;
class CQNewGLTexturesControl;
class CQNewGLTextureImage;
class CQNewGLTextureChooser;
class CQNewGLUVControl;
class CQNewGLAnimControl;
class CQNewGLBonesControl;
class CQNewGLTerrainControl;
class CQNewGLMazeControl;
class CQNewGLSkyboxControl;
class CQNewGLEmitterControl;
class CQNewGLFractalControl;
class CQNewGLDrawTreeControl;
class CQNewGLShapeControl;
class CQNewGLCanvas;
class CQNewGLUVMap;
class CQNewGLCamera;
class CQNewGLObjectsList;
class CQIconButton;

class CGeomObject3D;

class CQMatrix3D;
class CQPoint3DEdit;
class CQColorEdit;
class CQRealSpin;
class CQIntegerSpin;

class QTabWidget;
class QCheckBox;
class QListWidgetItem;
class QPushButton;
class QTimer;

//---

class CQNewGLControl : public QFrame {
  Q_OBJECT

 public:
  CQNewGLControl(CQNewGLModel *app);

  CQNewGLModel *app() const { return app_; }

  CQNewGLCanvas* canvas() const;
  CQNewGLUVMap*  uvMap() const;

  CQNewGLObjectsControl* objectsControl() const { return objectsControl_; }

  void updateGeneral();
  void updateObjects();
  void updateCameras();
  void updateLights();
  void updateNormals();
  void updateAxes();
  void updateBBox();
  void updateHull();
  void updateUV();
  void updateAnim();
  void updateBones();

  void updateTerrain();
  void updateMaze();
  void updateSkybox();
  void updateEmitter();
  void updateFractal();
  void updateDrawTree();
  void updateShape();

  CGeomObject3D *getRootObject() const;

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void modelTabSlot(int);

  void invalidateCameras();
  void invalidateLights();
  void invalidateTextures();
  void invalidateShapes();

  void updateTextures();

 private:
  using TabName = std::map<int, QString>;

  CQNewGLModel* app_ { nullptr };

  QTabWidget* tab_ { nullptr };

  QTabWidget* modelTab_ { nullptr };
  TabName     modelTabName_;

  QTabWidget* shapesTab_ { nullptr };
  TabName     shapesTabName_;

  CQNewGLGeneralControl*  generalControl_  { nullptr };
  CQNewGLCameraControl*   cameraControl_   { nullptr };
  CQNewGLLightsControl*   lightsControl_    { nullptr };
  CQNewGLNormalsControl*  normalsControl_  { nullptr };
  CQNewGLAxesControl*     axesControl_     { nullptr };
  CQNewGLBBoxControl*     bboxControl_     { nullptr };
  CQNewGLHullControl*     hullControl_     { nullptr };
  CQNewGLBasisControl*    basisControl_    { nullptr };
  CQNewGLObjectsControl*  objectsControl_  { nullptr };
  CQNewGLTexturesControl* texturesControl_ { nullptr };
  CQNewGLUVControl*       uvControl_       { nullptr };
  CQNewGLAnimControl*     animControl_     { nullptr };
  CQNewGLBonesControl*    bonesControl_    { nullptr };
  CQNewGLTerrainControl*  terrainControl_  { nullptr };
  CQNewGLMazeControl*     mazeControl_     { nullptr };
  CQNewGLSkyboxControl*   skyboxControl_   { nullptr };
  CQNewGLEmitterControl*  emitterControl_  { nullptr };
  CQNewGLFractalControl*  fractalControl_  { nullptr };
  CQNewGLDrawTreeControl* drawTreeControl_ { nullptr };
  CQNewGLShapeControl*    shapeControl_    { nullptr };
};

//---

class CQNewGLControlFrame : public QFrame {
  Q_OBJECT

 public:
  CQNewGLControlFrame(CQNewGLControl *control);

  CQNewGLCanvas *canvas() const;
  CQNewGLUVMap*  uvMap() const;

 protected:
  CQNewGLControl* control_ { nullptr };
};

//---

class CQNewGLGeneralControl : public CQNewGLControlFrame {
   Q_OBJECT

 public:
  CQNewGLGeneralControl(CQNewGLControl *control);

 private:
  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void bgColorSlot(const QColor &c);

  void depthTestSlot(int);
  void cullSlot(int);
  void frontFaceSlot(int);
  void polygonSolidSlot(int);
  void polygonLineSlot(int);
  void flipYZSlot(int);
  void invertDepthSlot(int);

  void typeSlot(int type);

 private:
  CQColorEdit* colorEdit_ { nullptr };

  QCheckBox* depthTestCheck_    { nullptr };
  QCheckBox* cullCheck_         { nullptr };
  QCheckBox* frontFaceCheck_    { nullptr };
  QCheckBox* polygonSolidCheck_ { nullptr };
  QCheckBox* polygonLineCheck_  { nullptr };
  QCheckBox* flipYZCheck_       { nullptr };
  QCheckBox* invertDepthCheck_  { nullptr };
};

//---

class CQNewGLCameraControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLCameraControl(CQNewGLControl *control);

  CQNewGLCamera *camera() const;

  void invalidateCameras();

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void showSlot(int b);
  void followSlot(int b);
  void eyelineSlot(int b);

  void orthoSlot(int);

  void cameraSelectedSlot(QListWidgetItem *, QListWidgetItem *);

  void updateCurrentSlot(int b);

  void rotateSlot(int b);
  void rotateAtSlot(int i);
  void strafeSlot(int b);

  void zoomSlot(double r);
  void nearSlot(double r);
  void farSlot(double r);
  void yawSlot(double r);
  void pitchSlot(double r);
  void rollSlot(double r);

  void speedSlot(double r);

  void positionSlot();
  void originSlot();
  void upSlot();
  void rightSlot();

  void colorSlot(const QColor &c);

  void addSlot();
  void resetSlot();
  void initSlot();

 private:
  int  currentCamera_ { 0 };
  bool updateCurrent_ { true };

  bool camerasInvalid_ { true };

  QCheckBox* showCheck_    { nullptr };
  QCheckBox* followCheck_  { nullptr };
  QCheckBox* eyelineCheck_ { nullptr };

  QCheckBox* orthoCheck_ { nullptr };

  QListWidget* camerasList_ { nullptr };

  QCheckBox* updateCurrentCheck_ { nullptr };

  QCheckBox* rotateCheck_   { nullptr };
  QComboBox* rotateAtCombo_ { nullptr };
  QCheckBox* strafeCheck_   { nullptr };

  CQRealSpin* zoomEdit_  { nullptr };
  CQRealSpin* nearEdit_  { nullptr };
  CQRealSpin* farEdit_   { nullptr };
  CQRealSpin* yawEdit_   { nullptr };
  CQRealSpin* pitchEdit_ { nullptr };
  CQRealSpin* rollEdit_  { nullptr };

  CQRealSpin* speedEdit_  { nullptr };

  CQPoint3DEdit* originEdit_   { nullptr };
  CQPoint3DEdit* positionEdit_ { nullptr };
  CQPoint3DEdit* upEdit_       { nullptr };
  CQPoint3DEdit* rightEdit_    { nullptr };

  CQMatrix3D* matrixEdit_ { nullptr };

  CQColorEdit* colorEdit_ { nullptr };
};

//---

class CQNewGLNormalsControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLNormalsControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void show1Slot(int);
  void show2Slot(int);
  void sizeSlot(double r);
  void colorSlot(const QColor &c);
  void tangentSpaceSlot(int);

 private:
  QCheckBox*   show1Check_        { nullptr };
  QCheckBox*   show2Check_        { nullptr };
  CQRealSpin*  sizeEdit_          { nullptr };
  CQColorEdit* colorEdit_         { nullptr };
  QCheckBox*   tangentSpaceCheck_ { nullptr };
};

//---

class CQNewGLLightsControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLLightsControl(CQNewGLControl *control);

  void invalidateLights();

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void ambientSlot(const QColor &c);
  void diffuseSlot(const QColor &c);
  void emissionSlot(const QColor &c);

  void ambientFactorSlot  (double v);
  void diffuseFactorSlot  (double v);
  void specularFactorSlot (double v);
  void shininessFactorSlot(double v);
  void emissiveFactorSlot (double v);

  void lightSelectedSlot(QListWidgetItem *, QListWidgetItem *);

  void showSlot(int);
  void typeSlot(int);
  void enabledSlot(int);
  void positionSlot();
  void directionSlot();
  void colorSlot(const QColor &c);
  void radiusSlot(double);
  void cutoffSlot(double);

  void addSlot();

 private:
  CQColorEdit* ambientEdit_  { nullptr };
  CQColorEdit* diffuseEdit_  { nullptr };
  CQColorEdit* emissionEdit_ { nullptr };

  QListWidget* lightsList_ { nullptr };

  bool lightsInvalid_ { true };

  QCheckBox*     showCheck_     { nullptr };
  QComboBox*     typeCombo_     { nullptr };
  QCheckBox*     enabledCheck_  { nullptr };
  CQPoint3DEdit* positionEdit_  { nullptr };
  CQPoint3DEdit* directionEdit_ { nullptr };
  CQColorEdit*   colorEdit_     { nullptr };
  CQRealSpin*    radiusEdit_    { nullptr };
  CQRealSpin*    cutoffEdit_    { nullptr };
};

//---

class CQNewGLAxesControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLAxesControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void showSlot(int b);

 private:
  QCheckBox* axisCheck_ { nullptr };
};

//---

class CQNewGLBBoxControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLBBoxControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void showSlot(int b);

 private:
  QCheckBox* showCheck_ { nullptr };
};

//---

class CQNewGLHullControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLHullControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void showSlot(int b);

 private:
  QCheckBox* showCheck_ { nullptr };
};

//---

class CQNewGLBasisControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLBasisControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void showSlot(int b);
  void sizeSlot(double r);
  void widthSlot(double r);

  void basisUSlot();
  void basisVSlot();
  void basisWSlot();

 private:
  QCheckBox*  showCheck_ { nullptr };
  CQRealSpin* sizeEdit_  { nullptr };
  CQRealSpin* widthEdit_ { nullptr };

  CQPoint3DEdit* basisUEdit_ { nullptr };
  CQPoint3DEdit* basisVEdit_ { nullptr };
  CQPoint3DEdit* basisWEdit_ { nullptr };
};

//---

class CQNewGLObjectsControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLObjectsControl(CQNewGLControl *control);

  void connectSlots(bool b);

  CGeomObject3D *getSelectedObject() const;
  CGeomObject3D *getObjectListSelected() const;

 public Q_SLOTS:
  void updateWidgets();

 private:
  void updateSelected(int ind);

 private Q_SLOTS:
  void updateModelMatrix();
  void objectSelectedSlot();

  void selectSlot();
  void deselectSlot();

  void showSlot();
  void hideSlot();

  void currentSlot();

  void centerSlot();
  void sizeSlot();

  void translateSlot();
  void rotateSlot();

  void diffuseMapSlot();
  void normalMapSlot();
  void specularMapSlot();
  void emissiveMapSlot();

  void addSlot();
  void resetSlot();

 private:
  CQNewGLObjectsList* objectsList_    { nullptr };
  QPushButton*        selectButton_   { nullptr };
  QPushButton*        deselectButton_ { nullptr };
  QPushButton*        showButton_     { nullptr };
  QPushButton*        hideButton_     { nullptr };
  QPushButton*        currentButton_  { nullptr };

  CQPoint3DEdit* centerEdit_    { nullptr };
  CQPoint3DEdit* sizeEdit_      { nullptr };
//CQPoint3DEdit* translateEdit_ { nullptr };
//CQPoint3DEdit* rotateEdit_    { nullptr };

  CQNewGLTextureChooser* diffuseTextureEdit_  { nullptr };
  CQNewGLTextureChooser* normalTextureEdit_   { nullptr };
  CQNewGLTextureChooser* specularTextureEdit_ { nullptr };
  CQNewGLTextureChooser* emissiveTextureEdit_ { nullptr };
};

//---

class CQNewGLTexturesControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLTexturesControl(CQNewGLControl *control);

  void invalidateTextures();

  int textureInd() const { return textureInd_; }

  QString textureName() const;

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void textureSelectedSlot(QListWidgetItem *, QListWidgetItem *);
  void loadSlot();

 private:
  QListWidget* texturesList_ { nullptr };

  bool texturesInvalid_ { true };

  CQNewGLTextureImage* image_      { nullptr };
  int                  textureInd_ { -1 };
  QCheckBox*           flipCheck_  { nullptr };
  QPushButton*         loadButton_ { nullptr };
};

//---

class CQNewGLTextureImage : public QFrame {
  Q_OBJECT

 public:
  CQNewGLTextureImage(CQNewGLTexturesControl *control);

  void paintEvent(QPaintEvent *) override;

 private:
  CQNewGLTexturesControl* control_ { nullptr };
};

//---

class CQNewGLUVControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLUVControl(CQNewGLControl *control);

  void updateObjects();

  void connectSlots(bool b);

 private Q_SLOTS:
  void objectSelectedSlot();
  void typeChanged(int);

 private:
  CQNewGLObjectsList* objectsList_ { nullptr };
  QComboBox*          typeCombo_   { nullptr };
};

//---

class CQNewGLAnimControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLAnimControl(CQNewGLControl *control);

//void updateObjects();

  void connectSlots(bool b);

 private:
  void step();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void enabledSlot(int);
  void bonesSlot(int);
  void showBonePointsSlot(int);
  void bonesTransformSlot(int);
//void objectSelectedSlot();
  void nameChanged(int);

  void timeSlot(double);
  void playSlot();
  void pauseSlot();
  void stepSlot();
  void timerSlot();

 private:
  bool running_ { false };

  QCheckBox* enabledCheck_        { nullptr };
  QCheckBox* bonesCheck_          { nullptr };
  QCheckBox* showBonePointsCheck_ { nullptr };

  QComboBox*          bonesTransform_ { nullptr };
//CQNewGLObjectsList* objectsList_    { nullptr };
  QComboBox*          nameCombo_      { nullptr };
  CQRealSpin*         timeEdit_       { nullptr };

  CQIconButton* playButton_  { nullptr };
  CQIconButton* pauseButton_ { nullptr };
  CQIconButton* stepButton_  { nullptr };
};

//---

class CQNewGLBonesControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  enum class MatrixType {
    INVERSE_BIND,
    LOCAL,
    GLOBAL,
    ANIM
  };

 public:
  CQNewGLBonesControl(CQNewGLControl *control);

  void updateApp();
  void updateMatrix();

  CMatrix3D getMatrix() const;

  void connectSlots(bool b);

  CGeomObject3D *getBonesObject() const;

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void boneSelectedSlot(QListWidgetItem *, QListWidgetItem *);
  void matrixTypeChanged(int);
  void inverseSlot(int);

  void animNameChanged(int ind);
  void animTimeSlot(double t);

 private:
  QListWidget* bonesList_     { nullptr };
  QComboBox*   matrixCombo_   { nullptr };
  QComboBox*   animNameCombo_ { nullptr };
  CQRealSpin*  animTimeEdit_  { nullptr };
  QCheckBox*   inverseCheck_  { nullptr };
  CQMatrix3D*  matrixEdit_    { nullptr };
  int          boneInd_       { -1 };
  MatrixType   matrixType_    { MatrixType::INVERSE_BIND };
  QString      boneAnimName_;
  double       boneAnimTime_  { 0.0 };
  bool         inverse_       { false };
};

//---

class CQNewGLTerrainControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLTerrainControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void typeSlot(int);
  void widthSlot(double);
  void heightSlot(double);
  void gridSizeSlot(int);
  void textureSlot(int);
  void gridSlot(int);

  void generateSlot();

 private:
  QComboBox*     typeCombo_    { nullptr };
  CQRealSpin*    widthEdit_    { nullptr };
  CQRealSpin*    heightEdit_   { nullptr };
  CQIntegerSpin* gridSizeEdit_ { nullptr };
  QCheckBox*     textureCheck_ { nullptr };
  QCheckBox*     gridCheck_    { nullptr };
};

//---

class CQNewGLMazeControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLMazeControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void widthSlot(double);
  void heightSlot(double);

  void generateSlot();

 private:
  CQRealSpin* widthEdit_  { nullptr };
  CQRealSpin* heightEdit_ { nullptr };
};

//---

class CQNewGLSkyboxControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLSkyboxControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void dirNameSlot();
  void widthSlot(double);
  void wireframeSlot(int);

  void generateSlot();

 private:
  QLineEdit*  dirNameEdit_    { nullptr };
  CQRealSpin* widthEdit_      { nullptr };
  QCheckBox*  wireframeCheck_ { nullptr };
};

//---

class CQNewGLEmitterControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLEmitterControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void emitterSelectedSlot(QListWidgetItem *, QListWidgetItem *);

  void typeSlot(int);
  void enabledSlot(int);
  void runningSlot(int);
  void positionSlot();
  void minVelocitySlot();
  void maxVelocitySlot();
  void intervalSlot(int);
  void gravitySlot(double);
  void sizeSlot(double);
  void maxParticlesSlot(int);
  void colorSlot(const QColor &c);
  void wireframeSlot(int);
  void imageSlot();

  void addSlot();

 private:
  QListWidget*   emitterList_      { nullptr };
  QComboBox*     typeCombo_        { nullptr };
  QCheckBox*     enabledCheck_     { nullptr };
  QCheckBox*     runningCheck_     { nullptr };
  CQPoint3DEdit* positionEdit_     { nullptr };
  CQPoint3DEdit* minVelocityEdit_  { nullptr };
  CQPoint3DEdit* maxVelocityEdit_  { nullptr };
  CQIntegerSpin* intervalSpin_     { nullptr };
  CQRealSpin*    gravitySpin_      { nullptr };
  CQRealSpin*    sizeSpin_         { nullptr };
  CQIntegerSpin* maxParticlesSpin_ { nullptr };
  CQColorEdit*   colorEdit_        { nullptr };
  QCheckBox*     wireframeCheck_   { nullptr };
  QLineEdit*     imageEdit_        { nullptr };

  bool listInvalid_ { true };
};

//---

class CQNewGLFractalControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLFractalControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void typeSlot(int);
  void wireframeSlot(int);

  void generateSlot();

 private:
  QComboBox* typeCombo_      { nullptr };
  QCheckBox* wireframeCheck_ { nullptr };
};

//---

class CQNewGLDrawTreeControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLDrawTreeControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void generateSlot();
};

//---

class CQNewGLShapeControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLShapeControl(CQNewGLControl *control);

  void connectSlots(bool b);

  void invalidateShapes();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void shapeSelectedSlot(QListWidgetItem *, QListWidgetItem *);

  void showSlot(int);
  void typeSlot(int);
  void startSlot();
  void endSlot();
  void widthSlot(double);
  void colorSlot(const QColor &c);
  void wireframeSlot(int);
  void solidSlot(int);
  void textureSlot();

  void addSlot();
  void updateSlot();

 private:
  QListWidget*   shapesList_     { nullptr };
  QCheckBox*     showCheck_      { nullptr };
  QComboBox*     typeCombo_      { nullptr };
  CQPoint3DEdit* startEdit_      { nullptr };
  CQPoint3DEdit* endEdit_        { nullptr };
  CQRealSpin*    widthEdit_      { nullptr };
  CQColorEdit*   colorEdit_      { nullptr };
  QCheckBox*     wireframeCheck_ { nullptr };
  QCheckBox*     solidCheck_     { nullptr };

  CQNewGLTextureChooser* textureEdit_ { nullptr };

  bool reload_ { false };

  struct ShapeData {
    CQNewGLShape::Type type      { CQNewGLShape::Type::SPHERE };
    CPoint3D           start     { 0, 0, 0 };
    CPoint3D           end       { 1, 1, 1 };
    double             width     { 1 };
    CRGBA              color     { 0.2, 0.2, 0.8 };
    bool               wireframe { false };
    bool               solid     { true };
    QString            textureName;
  };

  ShapeData shapeData_;
};

//---

class CQNewGLObjectsList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLObjectsList(CQNewGLControl *control);

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CGeomObject3D *getSelectedIndObject() const;
  CGeomObject3D *getObjectListSelected() const;

 public Q_SLOTS:
  void updateObjects();

 private Q_SLOTS:
  void objectSelectedSlot(QTreeWidgetItem *, QTreeWidgetItem *);

 private:
  QTreeWidgetItem *createObjectItem(CGeomObject3D *object);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  using ObjectItem = std::map<CGeomObject3D *, QTreeWidgetItem *>;

  CQNewGLControl* control_     { nullptr };
  QTreeWidget*    tree_        { nullptr };
  ObjectItem      objectItem_;
  int             selectedInd_ { -1 };
};

//---

class CQNewGLTextureChooser : public QComboBox {
  Q_OBJECT

 public:
  CQNewGLTextureChooser(CQNewGLControl *control);

  QString textureName() const { return textureName_; }
  void setTextureName(const QString &name);

  void connectSlots(bool b);

 Q_SIGNALS:
  void textureChanged();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void needsUpdateSlot();
  void currentIndexChanged(int);

 private:
  CQNewGLControl* control_ { nullptr };
  QString         textureName_;
  bool            needsUpdate_ { true };
};

#endif
