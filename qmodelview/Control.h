#ifndef Control_H
#define Control_H

#include <QFrame>

class CQRealSpin;

class QCheckBox;

namespace CQModel3DView {

class App;
class Camera;

class Control : public QFrame {
  Q_OBJECT

 public:
  Control(App *app);

 private:
  struct UpdateScope {
    UpdateScope(Control *control) : control_(control) { control_->connectSlots(false); }

   ~UpdateScope() { control_->connectSlots(true); control_->updateWidgets(); }

    Control* control_ { nullptr };
  };

  friend class UpdateScope;

  void connectSlots(bool);

  Camera *camera() const;

 private Q_SLOTS:
  void updateWidgets();

  void disableRollSlot(int);

  void clampPitchSlot(int);
  void minPitchSlot(double);
  void maxPitchSlot(double);

  void clampYawSlot(int);
  void minYawSlot(double);
  void maxYawSlot(double);

  void clampRollSlot(int);
  void minRollSlot(double);
  void maxRollSlot(double);

  void xOriginSlot(double);
  void yOriginSlot(double);
  void zOriginSlot(double);

  void xPosSlot(double);
  void yPosSlot(double);
  void zPosSlot(double);

  void distanceSlot(double);

  void pitchSlot(double);
  void yawSlot(double);
  void rollSlot(double);

  void nearSlot(double);
  void farSlot(double);
  void fovSlot(double);

 private:
  App* app_ { nullptr };

  QCheckBox* disableRollCheck_ { nullptr };

  QCheckBox*  clampPitchCheck_ { nullptr };
  CQRealSpin* minPitchEdit_    { nullptr };
  CQRealSpin* maxPitchEdit_    { nullptr };

  QCheckBox*  clampYawCheck_ { nullptr };
  CQRealSpin* minYawEdit_    { nullptr };
  CQRealSpin* maxYawEdit_    { nullptr };

  QCheckBox*  clampRollCheck_ { nullptr };
  CQRealSpin* minRollEdit_    { nullptr };
  CQRealSpin* maxRollEdit_    { nullptr };

  CQRealSpin* xOriginEdit_ { nullptr };
  CQRealSpin* yOriginEdit_ { nullptr };
  CQRealSpin* zOriginEdit_ { nullptr };

  CQRealSpin* xPosEdit_ { nullptr };
  CQRealSpin* yPosEdit_ { nullptr };
  CQRealSpin* zPosEdit_ { nullptr };

  CQRealSpin* distanceEdit_ { nullptr };

  CQRealSpin* pitchEdit_ { nullptr };
  CQRealSpin* yawEdit_   { nullptr };
  CQRealSpin* rollEdit_  { nullptr };

  CQRealSpin* nearEdit_ { nullptr };
  CQRealSpin* farEdit_  { nullptr };

  CQRealSpin* fovEdit_ { nullptr };
};

}

#endif
