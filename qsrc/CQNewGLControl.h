#ifndef CQNewGLControl_H
#define CQNewGLControl_H

#include <QFrame>

class CQNewGLModel;
class CQNewGLCameraControl;
class CQNewGLLightControl;
class CQNewGLObjectsControl;

class CQColorEdit;
class QCheckBox;
class QTabWidget;

class CQNewGLControl : public QFrame {
  Q_OBJECT

 public:
  CQNewGLControl(CQNewGLModel *app);

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

class CQNewGLCameraControl : public QFrame {
 public:
  CQNewGLCameraControl(CQNewGLControl *control);
};

class CQNewGLLightControl : public QFrame {
 public:
  CQNewGLLightControl(CQNewGLControl *control);
};

class CQNewGLObjectsControl : public QFrame {
 public:
  CQNewGLObjectsControl(CQNewGLControl *control);
};

#endif
