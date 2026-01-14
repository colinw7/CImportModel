#ifndef CQNewGLAnimChooser_H
#define CQNewGLAnimChooser_H

#include <QComboBox>

class CQNewGLModel;

class CQNewGLAnimChooser : public QComboBox {
  Q_OBJECT

 public:
  CQNewGLAnimChooser(CQNewGLModel *app);

  QString animName() const { return animName_; }
  void setAnimName(const QString &name);

  double tmin() const { return tmin_; }
  double tmax() const { return tmax_; }

  void connectSlots(bool b);

 Q_SIGNALS:
  void animChanged();

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void needsUpdateSlot();
  void currentIndexChanged(int);

 private:
  CQNewGLModel* app_ { nullptr };
  QString       animName_;
  bool          needsUpdate_ { true };
  double        tmin_ { 0.0 };
  double        tmax_ { 1.0 };
};

#endif
