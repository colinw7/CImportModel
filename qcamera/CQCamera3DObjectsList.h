#ifndef CQCamera3DObjectsList_H
#define CQCamera3DObjectsList_H

#include <QFrame>

class CQCamera3DCanvas;

class CQTreeWidget;
class CGeomObject3D;

class QTreeWidgetItem;

class CQCamera3DObjectsList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DObjectsList(CQCamera3DCanvas *canvas);

  void connectSlots(bool b);

  int selectedInd() const { return selectedInd_; }

  CGeomObject3D *getSelectedIndObject() const;
  CGeomObject3D *getObjectListSelected() const;

 public Q_SLOTS:
  void updateObjects();

 private Q_SLOTS:
  void objectSelectedSlot(QTreeWidgetItem *, QTreeWidgetItem *);

 private:
  QTreeWidgetItem *createObjectItem(CGeomObject3D *object);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  using ObjectItem = std::map<CGeomObject3D *, QTreeWidgetItem *>;

  CQCamera3DCanvas* canvas_      { nullptr };
  CQTreeWidget*     tree_        { nullptr };
  ObjectItem        objectItem_;
  int               selectedInd_ { -1 };
};

#endif
