#ifndef CQCamera3DCameraList_H
#define CQCamera3DCameraList_H

#include <QFrame>

class CQCamera3DCanvas;
class CQCamera3DCamera;

class QListWidget;
class QListWidgetItem;

class CQCamera3DCameraList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DCameraList(CQCamera3DCanvas *canvas);

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CQCamera3DCamera *getCurrentCamera() const;
  CQCamera3DCamera *getSelectedCamera() const;

 public Q_SLOTS:
  void updateCameras();

  void invalidate() { valid_ = false; updateCameras(); }

 private Q_SLOTS:
  void itemSelectedSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQCamera3DCanvas* canvas_      { nullptr };
  QListWidget*      list_        { nullptr };
  int               selectedInd_ { -1 };
  bool              valid_       { false };
};

#endif
