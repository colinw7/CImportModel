#ifndef CQCamera3DAnimObjectChooser_H
#define CQCamera3DAnimObjectChooser_H

#include <QComboBox>

class CQCamera3DApp;
class CQCamera3DGeomObject;

class CQCamera3DAnimObjectChooser : public QComboBox {
  Q_OBJECT

 public:
  CQCamera3DAnimObjectChooser(CQCamera3DApp *app);

  int objectInd() const { return objectInd_; }

  void setObject(CQCamera3DGeomObject *object);

  void connectSlots(bool b);

 Q_SIGNALS:
  void objectChanged();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void needsUpdateSlot();
  void currentIndexChanged(int);

 private:
  CQCamera3DApp* app_ { nullptr };
  int            objectInd_ { -1 };
  bool           needsUpdate_ { true };
};

#endif
