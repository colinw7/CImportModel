#ifndef CQCamera3DMaterialChooser_H
#define CQCamera3DMaterialChooser_H

#include <QComboBox>

class CQCamera3DApp;

class CQCamera3DMaterialChooser : public QComboBox {
  Q_OBJECT

 public:
  CQCamera3DMaterialChooser(CQCamera3DApp *app);

  QString materialName() const { return materialName_; }
  void setMaterialName(const QString &name);

  void connectSlots(bool b);

 Q_SIGNALS:
  void materialChanged();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void needsUpdateSlot();
  void currentIndexChanged(int);

 private:
  CQCamera3DApp* app_         { nullptr };
  QString        materialName_;
  bool           needsUpdate_ { true };
};

#endif
