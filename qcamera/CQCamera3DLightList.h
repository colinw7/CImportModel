#ifndef CQCamera3DLightList_H
#define CQCamera3DLightList_H

#include <QFrame>

class CQCamera3DCanvas;
class CQCamera3DLight;

class QListWidget;
class QListWidgetItem;

class CQCamera3DLightList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DLightList(CQCamera3DCanvas *canvas);

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CQCamera3DLight *getCurrentLight() const;
  CQCamera3DLight *getSelectedLight() const;

 public Q_SLOTS:
  void updateLights();

  void invalidate() { valid_ = false; updateLights(); }

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
