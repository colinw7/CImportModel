#ifndef Control_H
#define Control_H

#include <QFrame>

class CQRealSpin;

class QComboBox;

namespace CQShaderGen {

class App;

class Control : public QFrame {
  Q_OBJECT

 public:
  Control(App *app);

  void connectSlots(bool b);

 private Q_SLOTS:
  void updateWidgets();

  void dimSlot(int);
  void type2DSlot(int);
  void type3DSlot(int);

  void cameraXSlot(double r);
  void cameraYSlot(double r);
  void cameraZSlot(double r);

  void lightXSlot(double r);
  void lightYSlot(double r);
  void lightZSlot(double r);

 private:
  App* app_ { nullptr };

  QComboBox *dimCombo_    { nullptr };
  QComboBox *type2DCombo_ { nullptr };
  QComboBox *type3DCombo_ { nullptr };

  CQRealSpin* cameraXEdit_ { nullptr };
  CQRealSpin* cameraYEdit_ { nullptr };
  CQRealSpin* cameraZEdit_ { nullptr };

  CQRealSpin* lightXEdit_ { nullptr };
  CQRealSpin* lightYEdit_ { nullptr };
  CQRealSpin* lightZEdit_ { nullptr };
};

}

#endif
