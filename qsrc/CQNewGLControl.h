#ifndef CQNewGLControl_H
#define CQNewGLControl_H

#include <CMatrix3D.h>

#include <QFrame>
#include <QComboBox>
#include <QListWidget>

class CQNewGLModel;
class CQNewGLGeneralControl;
class CQNewGLCameraControl;
class CQNewGLLightControl;
class CQNewGLObjectsControl;
class CQNewGLTexturesControl;
class CQNewGLTextureImage;
class CQNewGLTextureChooser;
class CQNewGLUVControl;
class CQNewGLAnimControl;
class CQNewGLBonesControl;
class CQNewGLCanvas;
class CQNewGLUVMap;
class CQNewGLCamera;
class CQNewGLObjectsList;
class CQIconButton;

class CGeomObject3D;

class CQColorEdit;
class CQRealSpin;
class CQPoint3DEdit;
class CQMatrix3D;

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

  void updateWidgets();

  void updateCamera();
  void updateLights();
  void updateObjects();
  void updateTextures();
  void updateUV();
  void updateAnim();
  void updateBones();

  CGeomObject3D *getRootObject() const;

 private Q_SLOTS:
  void tabSlot(int);

 private:
  CQNewGLModel* app_ { nullptr };

  QTabWidget* tab_ { nullptr };

  CQNewGLGeneralControl*  generalControl_  { nullptr };
  CQNewGLCameraControl*   cameraControl_   { nullptr };
  CQNewGLLightControl*    lightControl_    { nullptr };
  CQNewGLObjectsControl*  objectsControl_  { nullptr };
  CQNewGLTexturesControl* texturesControl_ { nullptr };
  CQNewGLUVControl*       uvControl_       { nullptr };
  CQNewGLAnimControl*     animControl_     { nullptr };
  CQNewGLBonesControl*    bonesControl_    { nullptr };
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

 private Q_SLOTS:
  void bgColorSlot(const QColor &c);

  void depthTestSlot(int);
  void cullSlot(int);
  void frontFaceSlot(int);
  void polygonLineSlot(int);
  void showNormalsSlot(int);
  void flipYZSlot(int);
  void axisSlot(int);
  void invertDepthSlot(int);
  void orthoSlot(int);
  void uvMapSlot(int);

  void ambientSlot(const QColor &c);
  void diffuseSlot(const QColor &c);
  void emissionSlot(const QColor &c);

  void ambientFactorSlot  (double v);
  void diffuseFactorSlot  (double v);
  void specularFactorSlot (double v);
  void shininessFactorSlot(double v);
  void emissiveFactorSlot (double v);

  void typeSlot(int type);

 private:
  CQColorEdit* colorEdit_ { nullptr };

  QCheckBox* depthTestCheck_   { nullptr };
  QCheckBox* cullCheck_        { nullptr };
  QCheckBox* frontFaceCheck_   { nullptr };
  QCheckBox* polygonLineCheck_ { nullptr };
  QCheckBox* showNormalsCheck_ { nullptr };
  QCheckBox* flipYZCheck_      { nullptr };
  QCheckBox* axisCheck_        { nullptr };
  QCheckBox* invertDepthCheck_ { nullptr };
  QCheckBox* orthoCheck_       { nullptr };

  CQColorEdit* ambientEdit_  { nullptr };
  CQColorEdit* diffuseEdit_  { nullptr };
  CQColorEdit* emissionEdit_ { nullptr };
};

//---

class CQNewGLCameraControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLCameraControl(CQNewGLControl *control);

  CQNewGLCamera *camera() const;

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void rotateSlot(int b);
  void zoomSlot(double r);
  void nearSlot(double r);
  void farSlot(double r);
  void yawSlot(double r);
  void pitchSlot(double r);
  void rollSlot(double r);
  void posSlot();
  void resetSlot();

 private:
  QCheckBox*     rotateCheck_ { nullptr };
  CQRealSpin*    zoomEdit_    { nullptr };
  CQRealSpin*    nearEdit_    { nullptr };
  CQRealSpin*    farEdit_     { nullptr };
  CQRealSpin*    yawEdit_     { nullptr };
  CQRealSpin*    pitchEdit_   { nullptr };
  CQRealSpin*    rollEdit_    { nullptr };
  CQPoint3DEdit* posEdit_     { nullptr };
  QPushButton*   resetButton_ { nullptr };
};

//---

class CQNewGLLightControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLLightControl(CQNewGLControl *control);

  void updateWidgets(bool reload=true);

  void connectSlots(bool b);

 private Q_SLOTS:
  void lightSelectedSlot(QListWidgetItem *, QListWidgetItem *);
  void posSlot();
  void colorSlot(const QColor &c);
  void addSlot();

 private:
  QListWidget*   lightsList_ { nullptr };
  CQPoint3DEdit* posEdit_    { nullptr };
  CQColorEdit*   colorEdit_  { nullptr };
};

//---

class CQNewGLObjectsControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLObjectsControl(CQNewGLControl *control);

  void updateWidgets();

  void connectSlots(bool b);

  CGeomObject3D *getSelectedObject() const;
  CGeomObject3D *getObjectListSelected() const;

 private:
  void updateSelected(int ind);

 private Q_SLOTS:
  void updateModelMatrix();
  void objectSelectedSlot();

  void selectSlot();
  void deselectSlot();

  void showSlot();
  void hideSlot();

  void centerSlot();
  void sizeSlot();

  void translateSlot();
  void rotateSlot();

  void diffuseMapSlot();
  void normalMapSlot();
  void specularMapSlot();
  void emissiveMapSlot();

 private:
  CQNewGLObjectsList*    objectsList_     { nullptr };
  QPushButton*           selectButton_    { nullptr };
  QPushButton*           deselectButton_  { nullptr };
  QPushButton*           showButton_      { nullptr };
  QPushButton*           hideButton_      { nullptr };
  CQPoint3DEdit*         centerEdit_      { nullptr };
  CQPoint3DEdit*         sizeEdit_        { nullptr };
  CQPoint3DEdit*         translateEdit_   { nullptr };
  CQPoint3DEdit*         rotateEdit_      { nullptr };
  CQNewGLTextureChooser* diffuseMapEdit_  { nullptr };
  CQNewGLTextureChooser* normalMapEdit_   { nullptr };
  CQNewGLTextureChooser* specularMapEdit_ { nullptr };
  CQNewGLTextureChooser* emissiveMapEdit_ { nullptr };
};

//---

class CQNewGLTexturesControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLTexturesControl(CQNewGLControl *control);

  int textureInd() const { return textureInd_; }

  void updateWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void textureSelectedSlot(QListWidgetItem *, QListWidgetItem *);
  void loadSlot();

 private:
  QListWidget*         texturesList_ { nullptr };
  CQNewGLTextureImage* image_        { nullptr };
  int                  textureInd_   { -1 };
  QCheckBox*           flipCheck_    { nullptr };
  QPushButton*         loadButton_   { nullptr };
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

class CQNewGLTextureChooser : public QComboBox {
  Q_OBJECT

 public:
  CQNewGLTextureChooser(CQNewGLControl *control);

  QString textureName() const { return textureName_; }

  void updateWidgets();

  void connectSlots(bool b);

 Q_SIGNALS:
  void textureChanged();

 private Q_SLOTS:
  void currentIndexChanged(int);

 private:
  CQNewGLControl* control_ { nullptr };
  QString         textureName_;
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

  void updateWidgets();
//void updateObjects();

  void connectSlots(bool b);

 private:
  void step();

 private Q_SLOTS:
  void enabledSlot(int);
  void bonesSlot(int);
  void bonesTransformSlot(int);
//void objectSelectedSlot();
  void nameChanged(int);
  void timeSlot(double);
  void playSlot();
  void pauseSlot();
  void stepSlot();
  void timerSlot();

 private:
  QCheckBox*          enabledCheck_   { nullptr };
  QCheckBox*          bonesCheck_     { nullptr };
  QComboBox*          bonesTransform_ { nullptr };
//CQNewGLObjectsList* objectsList_    { nullptr };
  QComboBox*          nameCombo_      { nullptr };
  CQRealSpin*         timeEdit_       { nullptr };

  CQIconButton* playButton_  { nullptr };
  CQIconButton* pauseButton_ { nullptr };
  CQIconButton* stepButton_  { nullptr };

  QTimer* timer_ { nullptr };
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

  void updateWidgets();

  void updateApp();
  void updateMatrix();

  CMatrix3D getMatrix() const;

  void connectSlots(bool b);

  CGeomObject3D *getBonesObject() const;

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

class CQNewGLObjectsList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLObjectsList(CQNewGLControl *control);

  void updateObjects();

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CGeomObject3D *getSelectedIndObject() const;
  CGeomObject3D *getObjectListSelected() const;

 private Q_SLOTS:
  void objectSelectedSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQNewGLControl* control_     { nullptr };
  QListWidget*    list_        { nullptr };
  int             selectedInd_ { -1 };
};

#endif
