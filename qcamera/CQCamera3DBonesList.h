#ifndef CQCamera3DBonesList_H
#define CQCamera3DBonesList_H

#include <CPoint3D.h>

#include <QFrame>

class CQCamera3DApp;

class CGeomObject3D;
class CGeomNodeData;

class QTreeWidget;
class QTreeWidgetItem;

class CQCamera3DBonesList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DBonesList(CQCamera3DApp *app);

  int boneInd() const { return boneInd_; }

  void connectSlots(bool b);

  CGeomNodeData *currentBoneNode() const;
  CGeomNodeData *getBoneNode(int boneInd) const;

 public Q_SLOTS:
  void updateWidgets();

 private Q_SLOTS:
  void currentItemSlot(QTreeWidgetItem *, QTreeWidgetItem *);

 Q_SIGNALS:
  void currentItemChanged();

 private:
  struct BoneData {
    std::string           name;
    int                   ind { 0 };
    std::vector<CPoint3D> vertices;
  };

  void getBoneData(CGeomObject3D *object, int boneId, BoneData &boneData) const;
  void getNodeVertices(CGeomObject3D *object, int nodeId, BoneData &boneData) const;

  //---

  QTreeWidgetItem *createNodeItem(CGeomObject3D *object, int nodeId);

 private:
  using NodeItems = std::map<int, QTreeWidgetItem *>;

  CQCamera3DApp* app_  { nullptr };
  QTreeWidget*   tree_ { nullptr };

  int boneInd_ { -1 };

  NodeItems nodeItems_;
};

#endif
