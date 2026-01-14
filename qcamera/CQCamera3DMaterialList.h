#ifndef CQCamera3DMaterialList_H
#define CQCamera3DMaterialList_H

#include <QFrame>

class CQCamera3DApp;
class CGeomMaterial;

class QListWidget;
class QListWidgetItem;

class CQCamera3DMaterialList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DMaterialList(CQCamera3DApp *app);

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CGeomMaterial *getCurrentMaterial() const;
  CGeomMaterial *getSelectedMaterial() const;

 public Q_SLOTS:
  void updateMaterials();

  void invalidate() { valid_ = false; }

 private Q_SLOTS:
  void needsUpdateSlot();

  void itemSelectedSlot(QListWidgetItem *, QListWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  CQCamera3DApp* app_         { nullptr };
  QListWidget*   list_        { nullptr };
  int            selectedInd_ { -1 };
  bool           valid_       { false };
};

#endif
