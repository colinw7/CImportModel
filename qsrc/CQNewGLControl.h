#ifndef CQNewGLControl_H
#define CQNewGLControl_H

#include <CQNewGLShape.h>

#include <CMatrix3D.h>

#include <QFrame>
#include <QListWidget>
#include <QTreeWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>

class CQNewGLModel;
class CQNewGLGeneralControl;
class CQNewGLCameraControl;
class CQNewGLLightsControl;
class CQNewGLAnnotationsControl;
class CQNewGLAxesControl;
//class CQNewGLBBoxControl;
//class CQNewGLHullControl;
class CQNewGLObjectsControl;
class CQNewGLTexturesControl;
class CQNewGLTextureImage;
class CQNewGLTextureChooser;
class CQNewGLAnimControl;
class CQNewGLBonesControl;
class CQNewGLTerrainControl;
class CQNewGLMazeControl;
class CQNewGLSkyboxControl;
class CQNewGLEmitterControl;
class CQNewGLDrawTreeControl;
class CQNewGLShapeControl;
class CQNewGLCanvas;
class CQNewGLUVMap;
class CQNewGLCamera;

class CQNewGLObjectsList;
class CQNewGLCamerasList;
class CQNewGLLightsList;
class CQNewGLBonesList;
class CQNewGLShapesList;

class CQIconButton;

class  CGeomObject3D;
struct CGeomNodeData;

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
  void updateAnnotations();
  void updateAxes();
//void updateBBox();
//void updateHull();
//void updateUV();
  void updateAnim();
  void updateBones();

  void updateTerrain();
  void updateMaze();
  void updateSkybox();
  void updateEmitter();
  void updateDrawTree();
  void updateShape();

//CGeomObject3D *getRootObject() const;

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void modelTabSlot(int);

  void invalidateCameras();
  void invalidateLights();
  void invalidateTextures();

  void updateTextures();

 private:
  using TabName = std::map<int, QString>;

  CQNewGLModel* app_ { nullptr };

  QTabWidget* tab_ { nullptr };

  QTabWidget* modelTab_ { nullptr };
  TabName     modelTabName_;

  QTabWidget* extrasTab_ { nullptr };
  TabName     extrasTabName_;

  CQNewGLGeneralControl*     generalControl_     { nullptr };
  CQNewGLCameraControl*      cameraControl_      { nullptr };
  CQNewGLLightsControl*      lightsControl_      { nullptr };
  CQNewGLAnnotationsControl* annotationsControl_ { nullptr };
  CQNewGLAxesControl*        axesControl_        { nullptr };
//CQNewGLBBoxControl*        bboxControl_        { nullptr };
//CQNewGLHullControl*        hullControl_        { nullptr };
  CQNewGLObjectsControl*     objectsControl_     { nullptr };
  CQNewGLTexturesControl*    texturesControl_    { nullptr };
  CQNewGLAnimControl*        animControl_        { nullptr };
  CQNewGLBonesControl*       bonesControl_       { nullptr };
  CQNewGLTerrainControl*     terrainControl_     { nullptr };
  CQNewGLMazeControl*        mazeControl_        { nullptr };
  CQNewGLSkyboxControl*      skyboxControl_      { nullptr };
  CQNewGLEmitterControl*     emitterControl_     { nullptr };
  CQNewGLDrawTreeControl*    drawTreeControl_    { nullptr };
  CQNewGLShapeControl*       shapeControl_       { nullptr };
};

//---

class CQNewGLFractalControl : public QFrame {
  Q_OBJECT

 public:
  CQNewGLFractalControl(CQNewGLModel *app);

  CQNewGLModel *app() const { return app_; }

  void connectSlots(bool b);

 private:
  struct LayoutData {
    LayoutData() { }

    bool vertical { false };

    LayoutData &setVertical(bool b=true) { vertical = b; return *this; }
  };

  template<typename WIDGET>
  WIDGET *addLabelEdit(const QString &label, WIDGET *w, const LayoutData &data=LayoutData()) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (data.vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    addWidget(frame);

    return w;
  }

  template<typename WIDGET>
  WIDGET *addWidget(WIDGET *w) {
    layout_->addWidget(w);

    return w;
  }

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void typeSlot(int);
  void wireframeSlot(int);
  void textureSlot(int);
  void axesSlot(int);
  void pointSizeSlot(double);

  void generateSlot();

 private:
  CQNewGLModel* app_            { nullptr };
  QVBoxLayout*  layout_         { nullptr };
  QComboBox*    typeCombo_      { nullptr };
  QCheckBox*    wireframeCheck_ { nullptr };
  QCheckBox*    textureCheck_   { nullptr };
  QCheckBox*    axesCheck_      { nullptr };
  CQRealSpin*   pointSizeEdit_  { nullptr };
};

//---

class CQNewGLControlFrame : public QFrame {
  Q_OBJECT

 public:
  struct LayoutData {
    LayoutData() { }

    bool vertical { false };

    LayoutData &setVertical(bool b=true) { vertical = b; return *this; }
  };

 public:
  CQNewGLControlFrame(CQNewGLControl *control);

  CQNewGLCanvas *canvas() const;
  CQNewGLUVMap*  uvMap() const;

  template<typename WIDGET>
  WIDGET *addLabelEdit(const QString &label, WIDGET *w, const LayoutData &data=LayoutData()) {
    auto *frame = new QFrame;

    QBoxLayout *layout1 = nullptr;
    if (data.vertical)
      layout1 = new QVBoxLayout(frame);
    else
      layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    addWidget(frame);

    return w;
  }

  template<typename WIDGET>
  WIDGET *addWidget(WIDGET *w) {
    layout_->addWidget(w);

    return w;
  }

  QGroupBox *startGroup(const QString &name) {
    assert(! groupBox_);

    groupBox_    = new QGroupBox(name);
    groupLayout_ = new QVBoxLayout(groupBox_);

    return addWidget(groupBox_);
  }

  void endGroup() {
    assert(groupBox_);

    groupBox_    = nullptr;
    groupLayout_ = nullptr;
  }

  template<typename WIDGET>
  WIDGET *addGroupLabelEdit(const QString &label, WIDGET *w) {
    auto *frame   = new QFrame;
    auto *layout1 = new QHBoxLayout(frame);

    layout1->addWidget(new QLabel(label));
    layout1->addWidget(w);

    addGroupWidget(frame);

    return w;
  }

  template<typename WIDGET>
  WIDGET *addGroupWidget(WIDGET *w) {
    assert(groupLayout_);

    groupLayout_->addWidget(w);

    return w;
  }

 protected:
  CQNewGLControl* control_ { nullptr };
  QVBoxLayout*    layout_  { nullptr };

  QGroupBox*   groupBox_    { nullptr };
  QVBoxLayout* groupLayout_ { nullptr };
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
  void showOrientSlot(int);

  void typeSlot(int type);

 private:
  CQColorEdit* colorEdit_ { nullptr };

  QCheckBox* depthTestCheck_  { nullptr };
  QCheckBox* cullCheck_       { nullptr };
  QCheckBox* frontFaceCheck_  { nullptr };
  QCheckBox* showOrientCheck_ { nullptr };
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
  bool camerasInvalid_ { true };

  QCheckBox* showCheck_    { nullptr };
  QCheckBox* followCheck_  { nullptr };
  QCheckBox* eyelineCheck_ { nullptr };

  QCheckBox* orthoCheck_ { nullptr };

  CQNewGLCamerasList* camerasList_ { nullptr };

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

class CQNewGLAnnotationsControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLAnnotationsControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void showNormalsSlot(int);
  void showNewNormalsSlot(int);
  void normalsSizeSlot(double r);
  void normalsColorSlot(const QColor &c);
  void tangentSpaceSlot(int);

  void showBBoxSlot(int);

  void showHullSlot(int);

  void showBasisSlot(int b);
  void basisSizeSlot(double r);
  void basisWidthSlot(double r);

  void basisUSlot();
  void basisVSlot();
  void basisWSlot();

  void showWireframeSlot(int);

 private:
  QCheckBox*   showNormalsCheck_    { nullptr };
  QCheckBox*   showNewNormalsCheck_ { nullptr };
  CQRealSpin*  normalsSizeEdit_     { nullptr };
  CQColorEdit* normalsColorEdit_    { nullptr };
  QCheckBox*   tangentSpaceCheck_   { nullptr };

  QCheckBox* showBBoxCheck_ { nullptr };

  QCheckBox* showHullCheck_ { nullptr };

  QCheckBox*     showBasisCheck_ { nullptr };
  CQRealSpin*    basisSizeEdit_  { nullptr };
  CQRealSpin*    basisWidthEdit_ { nullptr };
  CQPoint3DEdit* basisUEdit_     { nullptr };
  CQPoint3DEdit* basisVEdit_     { nullptr };
  CQPoint3DEdit* basisWEdit_     { nullptr };

  QCheckBox* showWireframeCheck_ { nullptr };
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

  CQNewGLLightsList* lightsList_ { nullptr };

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

#if 0
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
#endif

//---

#if 0
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
#endif

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

  void solidSlot(int);
  void wireframeSlot(int);

  void flipYZSlot(int);
  void invertDepthSlot(int);

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

  QCheckBox* solidCheck_     { nullptr };
  QCheckBox* wireframeCheck_ { nullptr };

  QCheckBox* flipYZCheck_      { nullptr };
  QCheckBox* invertDepthCheck_ { nullptr };

  CQPoint3DEdit* centerEdit_     { nullptr };
  CQPoint3DEdit* sizeEdit_       { nullptr };
//CQPoint3DEdit* translateEdit_  { nullptr };
//CQPoint3DEdit* rotateEdit_     { nullptr };

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

class CQNewGLUVControl : public QFrame {
  Q_OBJECT

 public:
  CQNewGLUVControl(CQNewGLModel *app);

  CQNewGLModel *app() const { return app_; }

  void updateObjects();

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void objectSelectedSlot();
  void typeChanged(int);

 private:
  CQNewGLModel* app_ { nullptr };

  QVBoxLayout* layout_ { nullptr };

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
  void boneVerticesSlot(int);
  void boneListSlot();
  void boneSelectSlot();
  void boneDeselectSlot();
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

  QCheckBox*  enabledCheck_ { nullptr };
  QComboBox*  nameCombo_    { nullptr };
  CQRealSpin* timeEdit_     { nullptr };

  QCheckBox*          bonesCheck_          { nullptr };
  QCheckBox*          boneVerticesCheck_   { nullptr };
  QCheckBox*          showBonePointsCheck_ { nullptr };
  CQNewGLBonesList*   bonesList_           { nullptr };
  QPushButton*        boneSelectButton_    { nullptr };
  QPushButton*        boneDeselectButton_  { nullptr };
  QComboBox*          bonesTransform_      { nullptr };
//CQNewGLObjectsList* objectsList_         { nullptr };

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

//CGeomObject3D *getBonesObject() const;

 public Q_SLOTS:
  void updateWidgets();
  void bonesListSlot();

 private Q_SLOTS:
  void matrixTypeChanged(int);
  void inverseSlot(int);

  void animNameChanged(int ind);
  void animTimeSlot(double t);

 private:
  CQNewGLBonesList* bonesList_     { nullptr };
  QComboBox*        matrixCombo_   { nullptr };
  QComboBox*        animNameCombo_ { nullptr };
  CQRealSpin*       animTimeEdit_  { nullptr };
  QCheckBox*        isJointCheck_  { nullptr };
  QCheckBox*        inverseCheck_  { nullptr };
  CQMatrix3D*       matrixEdit_    { nullptr };
  MatrixType        matrixType_    { MatrixType::INVERSE_BIND };
  QString           boneAnimName_;
  double            boneAnimTime_  { 0.0 };
  bool              inverse_       { false };
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
  void octavesSlot(int);
  void textureSlot(int);
  void gridSlot(int);

  void generateSlot();

 private:
  QComboBox*     typeCombo_    { nullptr };
  CQRealSpin*    widthEdit_    { nullptr };
  CQRealSpin*    heightEdit_   { nullptr };
  CQIntegerSpin* gridSizeEdit_ { nullptr };
  CQIntegerSpin* octavesEdit_  { nullptr };
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
  void maxAgeSlot(int);
  void startColorSlot(const QColor &c);
  void endColorSlot(const QColor &c);
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
  CQIntegerSpin* maxAgeSpin_       { nullptr };
  CQColorEdit*   startColorEdit_   { nullptr };
  CQColorEdit*   endColorEdit_     { nullptr };
  QCheckBox*     wireframeCheck_   { nullptr };
  QLineEdit*     imageEdit_        { nullptr };

  bool listInvalid_ { true };
};

//---

class CQNewGLDrawTreeControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLDrawTreeControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

  void widthSlot(double w);
  void heightSlot(double h);
  void leftDirectionSlot();
  void rightDirectionSlot();
  void leftAlphaSlot(double w);
  void rightAlphaSlot(double h);
  void depthSlot(int depth);

 private Q_SLOTS:
  void generateSlot();

 private:
  CQRealSpin*    widthEdit_          { nullptr };
  CQRealSpin*    heightEdit_         { nullptr };
  CQPoint3DEdit* leftDirectionEdit_  { nullptr };
  CQPoint3DEdit* rightDirectionEdit_ { nullptr };
  CQRealSpin*    leftAlphaEdit_      { nullptr };
  CQRealSpin*    rightAlphaEdit_     { nullptr };
  CQIntegerSpin* depthEdit_          { nullptr };

  double    width_          { 0.2 };
  double    height_         { 1.0 };
  CVector3D leftDirection_  {  2.0, 1.0, 0.0 };
  CVector3D rightDirection_ { -2.0, 1.0, 0.0 };
  double    leftAlpha_      { 1.1 };
  double    rightAlpha_     { 1.1 };
  int       depth_          { 6 };
};

//---

class CQNewGLShapeControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLShapeControl(CQNewGLControl *control);

  void connectSlots(bool b);

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void shapeSelectedSlot(QListWidgetItem *, QListWidgetItem *);

  void typeSlot(int);

  void showSlot(int);
  void wireframeSlot(int);
  void solidSlot(int);

  void startSlot();
  void endSlot();
  void widthSlot(double);
  void colorSlot(const QColor &c);
  void angleStartSlot(double);
  void angleDeltaSlot(double);
  void sizeSlot();
  void textureSlot();

  void addSlot();
  void updateSlot();
  void deleteSlot();

 private:
  void updateState();

  void updateShape(CQNewGLShape *shape);

 private:
  CQNewGLShapesList* shapesList_ { nullptr };

  QComboBox*             typeCombo_      { nullptr };
  QCheckBox*             showCheck_      { nullptr };
  QCheckBox*             wireframeCheck_ { nullptr };
  QCheckBox*             solidCheck_     { nullptr };
  CQPoint3DEdit*         startEdit_      { nullptr };
  CQPoint3DEdit*         endEdit_        { nullptr };
  CQRealSpin*            widthEdit_      { nullptr };
  CQColorEdit*           colorEdit_      { nullptr };
  CQRealSpin*            angleStartEdit_ { nullptr };
  CQRealSpin*            angleDeltaEdit_ { nullptr };
  CQPoint3DEdit*         sizeEdit_       { nullptr };
  CQNewGLTextureChooser* textureEdit_    { nullptr };

  QPushButton* addButton_    { nullptr };
  QPushButton* updateButton_ { nullptr };
  QPushButton* deleteButton_ { nullptr };

  struct ShapeData {
    CQNewGLShape::Type type       { CQNewGLShape::Type::SPHERE };
    CPoint3D           start      { 0, 0, 0 };
    CPoint3D           end        { 1, 1, 1 };
    double             width      { 1 };
    CRGBA              color      { 0.2, 0.2, 0.8 };
    double             angleStart { 0.0 };
    double             angleDelta { 2.0*M_PI };
    bool               wireframe  { false };
    bool               solid      { true };
    CPoint3D           size       { 1, 1, 1 };
    QString            textureName;
  };

  ShapeData shapeData_;
};

//---

class CQNewGLCamerasList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLCamerasList(CQNewGLControl *control);

  bool isUpdateCurrent() const { return updateCurrent_; }
  void setUpdateCurrent(bool b) { updateCurrent_ = b; }

  void connectSlots(bool b);

  void updateWidgets();

  CQNewGLCamera *getCurrentCamera() const;

 private Q_SLOTS:
  void currentItemSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQNewGLControl* control_ { nullptr };
  QListWidget*    list_    { nullptr };

  int  currentCamera_ { 0 };
  bool updateCurrent_ { true };
};

//---

class CQNewGLLightsList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLLightsList(CQNewGLControl *control);

  void connectSlots(bool b);

  void updateWidgets();

 private Q_SLOTS:
  void currentItemSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQNewGLControl* control_ { nullptr };
  QListWidget*    list_    { nullptr };
};

//---

class CQNewGLBonesList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLBonesList(CQNewGLControl *control);

  int boneInd() const { return boneInd_; }

  void connectSlots(bool b);

  void updateWidgets();

  CGeomNodeData *getBoneNode(int boneInd) const;

 private Q_SLOTS:
  void currentItemSlot(QTreeWidgetItem *, QTreeWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  QTreeWidgetItem *createNodeItem(CGeomObject3D *object, int nodeId);

 private:
  using NodeItems = std::map<int, QTreeWidgetItem *>;

  CQNewGLControl* control_ { nullptr };
  QTreeWidget*    tree_    { nullptr };

  int boneInd_ { -1 };

  NodeItems nodeItems_;
};

//---

#if 0
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

//---

class CQNewGLShapesList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLShapesList(CQNewGLControl *control);

  void connectSlots(bool b);

  void updateWidgets();

 private Q_SLOTS:
  void invalidateShapes();

  void currentItemSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQNewGLControl* control_ { nullptr };
  QListWidget*    list_    { nullptr };

  bool reload_ { false };
};

//---

#endif
