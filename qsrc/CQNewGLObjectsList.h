#ifndef CQNewGLObjectsList_H
#define CQNewGLObjectsList_H

#include <QFrame>

class CQNewGLCanvas;

class CGeomObject3D;

class QTreeWidget;
class QTreeWidgetItem;

class CQNewGLObjectsList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLObjectsList(CQNewGLCanvas *canvas);

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

  CQNewGLCanvas* canvas_      { nullptr };
  QTreeWidget*   tree_        { nullptr };
  ObjectItem     objectItem_;
  int            selectedInd_ { -1 };
};

#endif
