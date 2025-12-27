#ifndef CQNewGLCamerasList_H
#define CQNewGLCamerasList_H

#include <QFrame>

class CQNewGLControl;
class CQNewGLCamera;

class QListWidget;
class QListWidgetItem;

class CQNewGLCamerasList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLCamerasList(CQNewGLControl *control);

  bool isUpdateCurrent() const { return updateCurrent_; }
  void setUpdateCurrent(bool b) { updateCurrent_ = b; }

  void connectSlots(bool b);

  void updateWidgets();

  CQNewGLCamera *getCurrentCamera() const;

 private Q_SLOTS:
  void currentItemSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQNewGLControl* control_ { nullptr };
  QListWidget*    list_    { nullptr };

  int  currentCamera_ { 0 };
  bool updateCurrent_ { true };
};

#endif
