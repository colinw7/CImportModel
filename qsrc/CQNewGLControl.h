#ifndef CQNewGLControl_H
#define CQNewGLControl_H

#include <QFrame>

class CQNewGLModel;
class CQNewGLCameraControl;
class CQNewGLLightControl;
class CQNewGLObjectsControl;
class CQNewGLCanvas;

class CGLCamera;

class CQColorEdit;
class CQRealSpin;
class CQPoint3DEdit;
class QTabWidget;
class QListWidget;
class QCheckBox;
class QListWidgetItem;

class CQNewGLControl : public QFrame {
  Q_OBJECT

 public:
  CQNewGLControl(CQNewGLModel *app);

  CQNewGLModel *app() const { return app_; }

  void update();

 private Q_SLOTS:
  void bgColorSlot(const QColor &c);

  void depthTestSlot(int);
  void cullSlot(int);
  void frontFaceSlot(int);
  void polygonLineSlot(int);

  void ambientSlot  (double v);
  void diffuseSlot  (double v);
  void specularSlot (double v);
  void shininessSlot(double v);

  void typeSlot(int type);

 private:
  CQNewGLModel* app_ { nullptr };

  CQColorEdit* colorEdit_ { nullptr };

  QCheckBox* depthTestCheck_   { nullptr };
  QCheckBox* cullCheck_        { nullptr };
  QCheckBox* frontFaceCheck_   { nullptr };
  QCheckBox* polygonLineCheck_ { nullptr };

  QTabWidget* tab_ { nullptr };

  CQNewGLCameraControl*  cameraControl_   { nullptr };
  CQNewGLLightControl*   lightControl_   { nullptr };
  CQNewGLObjectsControl* objectsControl_ { nullptr };
};

class CQNewGLControlFrame : public QFrame {
  Q_OBJECT

 public:
  CQNewGLControlFrame(CQNewGLControl *control);

  CQNewGLCanvas *canvas() const;

 protected:
  CQNewGLControl* control_ { nullptr };
};

class CQNewGLCameraControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLCameraControl(CQNewGLControl *control);

  CGLCamera *camera() const;

 private Q_SLOTS:
  void rotateSlot(int b);
  void zoomSlot(double r);
  void nearSlot(double r);
  void farSlot(double r);
  void yawSlot(double r);
  void pitchSlot(double r);
  void rollSlot(double r);
  void posSlot();

 private:
  QCheckBox*     rotateCheck_ { nullptr };
  CQRealSpin*    zoomEdit_    { nullptr };
  CQRealSpin*    nearEdit_    { nullptr };
  CQRealSpin*    farEdit_     { nullptr };
  CQRealSpin*    yawEdit_     { nullptr };
  CQRealSpin*    pitchEdit_   { nullptr };
  CQRealSpin*    rollEdit_    { nullptr };
  CQPoint3DEdit* posEdit_     { nullptr };
};

class CQNewGLLightControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLLightControl(CQNewGLControl *control);
};

class CQNewGLObjectsControl : public CQNewGLControlFrame {
  Q_OBJECT

 public:
  CQNewGLObjectsControl(CQNewGLControl *control);

  void update();

 private Q_SLOTS:
  void objectSelectedSlot(QListWidgetItem *, QListWidgetItem *);

 private:
  QListWidget* objectsList_ { nullptr };
};

#endif
