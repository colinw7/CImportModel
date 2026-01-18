#ifndef CQCamera3DOptions_H
#define CQCamera3DOptions_H

#include <CQCamera3DCanvas.h>

class CQCamera3DUI;
class CQCamera3DAddMouseMode;
class CQCamera3DMoveMouseMode;
class CQCamera3DScaleMouseMode;
class CQCamera3DRotateMouseMode;
class CQCamera3DExtrudeMouseMode;

class CQRealSpin;
class CQIntegerSpin;
class QComboBox;

class CQCamera3DOptions : public QObject {
  Q_OBJECT

 public:
  CQCamera3DOptions() { }

  virtual ~CQCamera3DOptions() { }

  virtual void addOptions(CQCamera3DUI *) = 0;

  virtual QString title() const = 0;
};

class CQCamera3DAddOptions : public CQCamera3DOptions {
  Q_OBJECT

 public:
  CQCamera3DAddOptions(CQCamera3DAddMouseMode *mode);

  void addOptions(CQCamera3DUI *ui) override;

  QString title() const override;

  void connectSlots(bool b);

 private Q_SLOTS:
  void xSlot(double);
  void ySlot(double);
  void zSlot(double);

  void rxSlot(double);
  void rySlot(double);
  void rzSlot(double);

  void xSizeSlot(double);
  void ySizeSlot(double);
  void zSizeSlot(double);

  void innerRadiusSlot(double);
  void outerRadiusSlot(double);

  void circleTypeSlot(int);

  void verticesXSlot(int i);
  void verticesYSlot(int i);

  void startAngleSlot(double);
  void extentAngleSlot(double);

 private:
  CQCamera3DAddMouseMode* mode_ { nullptr };

  CQRealSpin* xEdit_ { nullptr };
  CQRealSpin* yEdit_ { nullptr };
  CQRealSpin* zEdit_ { nullptr };

  CQRealSpin* rxEdit_ { nullptr };
  CQRealSpin* ryEdit_ { nullptr };
  CQRealSpin* rzEdit_ { nullptr };

  CQRealSpin* radiusEdit_ { nullptr };

  CQRealSpin* xSizeEdit_ { nullptr };
  CQRealSpin* ySizeEdit_ { nullptr };
  CQRealSpin* zSizeEdit_ { nullptr };

  CQRealSpin* innerRadiusEdit_ { nullptr };
  CQRealSpin* outerRadiusEdit_ { nullptr };

  QComboBox* circleTypeCombo_ { nullptr };

  CQIntegerSpin* verticesXEdit_ { nullptr };
  CQIntegerSpin* verticesYEdit_ { nullptr };

  CQRealSpin* startAngleEdit_  { nullptr };
  CQRealSpin* extentAngleEdit_ { nullptr };
};

//---

class CQCamera3DMoveOptions : public CQCamera3DOptions {
  Q_OBJECT

 public:
  CQCamera3DMoveOptions(CQCamera3DMoveMouseMode *mode);

  void addOptions(CQCamera3DUI *ui) override;

  QString title() const override;

  void connectSlots(bool b);

 private Q_SLOTS:
  void dirSlot(int ind);

  void xSlot(double);
  void ySlot(double);
  void zSlot(double);

 private:
  CQCamera3DMoveMouseMode* mode_ { nullptr };

  QComboBox*  dirCombo_ { nullptr };
  CQRealSpin* xEdit_    { nullptr };
  CQRealSpin* yEdit_    { nullptr };
  CQRealSpin* zEdit_    { nullptr };
};

//---

class CQCamera3DScaleOptions : public CQCamera3DOptions {
  Q_OBJECT

 public:
  CQCamera3DScaleOptions(CQCamera3DScaleMouseMode *mode);

  void addOptions(CQCamera3DUI *ui) override;

  QString title() const override;

  void connectSlots(bool b);

 private Q_SLOTS:
  void dirSlot(int ind);

  void xSlot(double);
  void ySlot(double);
  void zSlot(double);

 private:
  CQCamera3DScaleMouseMode* mode_ { nullptr };

  QComboBox*  dirCombo_ { nullptr };
  CQRealSpin* xEdit_    { nullptr };
  CQRealSpin* yEdit_    { nullptr };
  CQRealSpin* zEdit_    { nullptr };
};

//---

class CQCamera3DRotateOptions : public CQCamera3DOptions {
  Q_OBJECT

 public:
  CQCamera3DRotateOptions(CQCamera3DRotateMouseMode *mode);

  void addOptions(CQCamera3DUI *ui) override;

  QString title() const override;

  void connectSlots(bool b);

 private Q_SLOTS:
  void dirSlot(int ind);

  void xSlot(double);
  void ySlot(double);
  void zSlot(double);

 private:
  CQCamera3DRotateMouseMode* mode_ { nullptr };

  QComboBox*  dirCombo_ { nullptr };
  CQRealSpin* xEdit_    { nullptr };
  CQRealSpin* yEdit_    { nullptr };
  CQRealSpin* zEdit_    { nullptr };
};

//---

class CQCamera3DExtrudeOptions : public CQCamera3DOptions {
  Q_OBJECT

 public:
  CQCamera3DExtrudeOptions(CQCamera3DExtrudeMouseMode *mode);

  void addOptions(CQCamera3DUI *ui) override;

  QString title() const override;

  void connectSlots(bool b);

 private Q_SLOTS:
  void dirSlot(int ind);

  void distanceSlot(double);

 private:
  CQCamera3DExtrudeMouseMode* mode_ { nullptr };

  QComboBox*  dirCombo_     { nullptr };
  CQRealSpin* distanceEdit_ { nullptr };
};

#endif
