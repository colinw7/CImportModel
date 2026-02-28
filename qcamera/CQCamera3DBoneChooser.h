#ifndef CQCamera3DBoneChooser_H
#define CQCamera3DBoneChooser_H

#include <CQCamera3DApp.h>

#include <QComboBox>

class CQCamera3DBoneChooser : public QComboBox {
  Q_OBJECT

 public:
  CQCamera3DBoneChooser(CQCamera3DApp *app);

  int objectInd() const { return objectInd_; }
  void setObjectInd(int i);

  int boneInd() const { return boneInd_; }
  void setBoneInd(int i);

  void connectSlots(bool b);

 Q_SIGNALS:
  void boneChanged();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void needsUpdateSlot();
  void currentIndexChanged(int);

 private:
  CQCamera3DApp* app_         { nullptr };
  int            objectInd_   { -1 };
  int            boneInd_     { -1 };
  bool           needsUpdate_ { true };
};

#endif
