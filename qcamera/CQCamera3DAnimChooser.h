#ifndef CQCamera3DAnimChooser_H
#define CQCamera3DAnimChooser_H

#include <CQCamera3DApp.h>

#include <QComboBox>

class CQCamera3DAnimChooser : public QComboBox {
  Q_OBJECT

 public:
  struct AnimData {
    QStringList names;
    QString     name;
    double      tmin     { 0.0 };
    double      tmax     { 0.0 };
    double      timeStep { 100.0 };
  };

 public:
  CQCamera3DAnimChooser(CQCamera3DApp *app);

  int objectInd() const { return objectInd_; }
  void setObjectInd(int i);

  QString animName() const { return animData_.name; }

  double tmin() const { return animData_.tmin; }
  double tmax() const { return animData_.tmax; }

  double timeStep() const { return animData_.timeStep; }

  CGeomObject3D *getObject() const;

 private:
  void connectSlots(bool b);

  void getAnimData(AnimData &animData) const;

 Q_SIGNALS:
  void animChanged();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void needsUpdateSlot();
  void currentIndexChanged(int);

 private:
  CQCamera3DApp* app_         { nullptr };
  int            objectInd_   { -1 };
  AnimData       animData_;
  bool           needsUpdate_ { true };
};

#endif
