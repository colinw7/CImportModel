#ifndef LightControl_H
#define LightControl_H

#include <QFrame>

class CQColorEdit;
class CQRealSpin;

class QComboBox;
class QCheckBox;
class QGroupBox;

class QListWidget;
class QListWidgetItem;

namespace CQTextureGen {

class App;
class Light;
class LightList;

class LightControl : public QFrame {
  Q_OBJECT

 public:
  LightControl(App *app);

  App *app() const { return app_; }

  void connectSlots(bool b);

  Light *currentLight() const;

 private Q_SLOTS:
  void updateWidgets();

  void ambientColorSlot(const QColor &);
  void ambientStrengthSlot(double);
  void diffuseStrengthSlot(double);
  void specularColorSlot(const QColor &);
  void specularStrengthSlot(double);
  void emissionColorSlot(const QColor &);
  void emissionStrengthSlot(double);
  void shininessSlot(double);

  void enabledSlot(int state);

  void typeSlot(int ind);

  void colorSlot(const QColor &c);

  void xPosSlot(double);
  void yPosSlot(double);
  void zPosSlot(double);

  void xDirSlot(double);
  void yDirSlot(double);
  void zDirSlot(double);

  void pointRadiusSlot(double);
  void attenuation0Slot(double);
  void attenuation1Slot(double);
  void attenuation2Slot(double);

  void spotXDirSlot(double);
  void spotYDirSlot(double);
  void spotZDirSlot(double);

  void addSlot();

 private:
  App* app_ { nullptr };

  CQColorEdit *ambientColorEdit_     { nullptr };
  CQRealSpin*  ambientStrengthEdit_  { nullptr };
  CQRealSpin*  diffuseStrengthEdit_  { nullptr };
  CQColorEdit* specularColorEdit_    { nullptr };
  CQRealSpin*  specularStrengthEdit_ { nullptr };
  CQColorEdit* emissionColorEdit_    { nullptr };
  CQRealSpin*  emissionStrengthEdit_ { nullptr };
  CQRealSpin*  shininessEdit_        { nullptr };

  LightList* lightList_ { nullptr };

  QCheckBox* enabledCheck_ { nullptr };

  QComboBox* typeCombo_ { nullptr };

  CQColorEdit* colorEdit_ { nullptr };

  CQRealSpin* xPosEdit_ { nullptr };
  CQRealSpin* yPosEdit_ { nullptr };
  CQRealSpin* zPosEdit_ { nullptr };

  QGroupBox* directionGroup_ { nullptr };

  CQRealSpin* xDirEdit_ { nullptr };
  CQRealSpin* yDirEdit_ { nullptr };
  CQRealSpin* zDirEdit_ { nullptr };

  QGroupBox*  pointGroup_       { nullptr };
  CQRealSpin* radiusEdit_       { nullptr };
  CQRealSpin* attenuation0Edit_ { nullptr };
  CQRealSpin* attenuation1Edit_ { nullptr };
  CQRealSpin* attenuation2Edit_ { nullptr };

  QGroupBox*  spotGroup_    { nullptr };
  CQRealSpin* expEdit_      { nullptr };
  CQRealSpin* cutOffEdit_   { nullptr };
  CQRealSpin* spotXDirEdit_ { nullptr };
  CQRealSpin* spotYDirEdit_ { nullptr };
  CQRealSpin* spotZDirEdit_ { nullptr };
};

//---

class LightList : public QFrame {
  Q_OBJECT

 public:
  LightList(LightControl *control);

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  Light *getCurrentLight() const;
  Light *getSelectedLight() const;

 public Q_SLOTS:
  void updateLights();

  void invalidate() { valid_ = false; }

 private Q_SLOTS:
  void needsUpdateSlot();

  void itemSelectedSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  LightControl* control_     { nullptr };
  QListWidget*  list_        { nullptr };
  int           selectedInd_ { -1 };
  bool          valid_       { false };
};

//---

}

#endif
