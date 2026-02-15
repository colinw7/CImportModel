#ifndef CameraControl_H
#define CameraControl_H

#include <QFrame>

class CQRealSpin;

namespace CQTextureGen {

class App;

class CameraControl : public QFrame {
  Q_OBJECT

 public:
  CameraControl(App *app);

  void connectSlots(bool b);

 private Q_SLOTS:
  void updateWidgets();

  void xPosSlot(double);
  void yPosSlot(double);
  void zPosSlot(double);

  void xOriginSlot(double);
  void yOriginSlot(double);
  void zOriginSlot(double);

  void fovSlot(double);
  void nearSlot(double);
  void farSlot(double);

 private:
  App* app_ { nullptr };

  CQRealSpin* xPosEdit_ { nullptr };
  CQRealSpin* yPosEdit_ { nullptr };
  CQRealSpin* zPosEdit_ { nullptr };

  CQRealSpin* xOriginEdit_ { nullptr };
  CQRealSpin* yOriginEdit_ { nullptr };
  CQRealSpin* zOriginEdit_ { nullptr };

  CQRealSpin* fovEdit_ { nullptr };
  CQRealSpin* nearEdit_ { nullptr };
  CQRealSpin* farEdit_ { nullptr };

};

}

#endif
