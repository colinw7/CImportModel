#ifndef Toolbar_H
#define Toolbar_H

#include <App.h>

#include <QFrame>

class App;

class CQIconButton;

class QCheckBox;

namespace CQSkybox3D {

class Toolbar : public QFrame {
  Q_OBJECT

 public:
  Toolbar(App *app);

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void editCameraSlot(bool state);
  void editModelSlot(bool state);
  void editLightSlot(bool state);

  void bufferSlot(int state);
  void lightSlot(int state);

 private:
  App* app_ { nullptr };

  CQIconButton* cameraButton_ { nullptr };
  CQIconButton* modelButton_  { nullptr };
  CQIconButton* lightButton_  { nullptr };

  QCheckBox* bufferCheck_ { nullptr };
  QCheckBox* lightCheck_  { nullptr };
};

}

#endif
