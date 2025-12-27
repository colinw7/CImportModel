#ifndef CQNewGLBonesList_H
#define CQNewGLBonesList_H

#include <QFrame>

class CQNewGLControl;

class CGeomObject3D;
class CGeomNodeData;

class QTreeWidget;
class QTreeWidgetItem;

class CQNewGLBonesList : public QFrame {
  Q_OBJECT

 public:
  CQNewGLBonesList(CQNewGLControl *control);

  int boneInd() const { return boneInd_; }

  void connectSlots(bool b);

  void updateWidgets();

  CGeomNodeData *getBoneNode(int boneInd) const;

 private Q_SLOTS:
  void currentItemSlot(QTreeWidgetItem *, QTreeWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  QTreeWidgetItem *createNodeItem(CGeomObject3D *object, int nodeId);

 private:
  using NodeItems = std::map<int, QTreeWidgetItem *>;

  CQNewGLControl* control_ { nullptr };
  QTreeWidget*    tree_    { nullptr };

  int boneInd_ { -1 };

  NodeItems nodeItems_;
};

#endif
