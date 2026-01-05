#ifndef CQCamera3DBonesList_H
#define CQCamera3DBonesList_H

#include <CPoint3D.h>

#include <QFrame>

class CQCamera3DApp;

class CQTreeWidget;
class CGeomObject3D;
class CGeomNodeData;

class QTreeWidgetItem;

class CQCamera3DBonesList : public QFrame {
  Q_OBJECT

 public:
  CQCamera3DBonesList(CQCamera3DApp *app);

  int objId() const { return objId_; }
  int boneInd() const { return boneInd_; }

  void connectSlots(bool b);

  CGeomNodeData *currentBoneNode() const;
  CGeomNodeData *getBoneNode(int objId, int boneInd) const;

  void setCurrentBoneNode(int objId, int nodeId);

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
  using NodeItems       = std::map<int, QTreeWidgetItem *>;
  using ObjectNodeItems = std::map<int, NodeItems>;

  CQCamera3DApp* app_  { nullptr };
  CQTreeWidget*  tree_ { nullptr };

  int objId_   { -1 };
  int boneInd_ { -1 };

  ObjectNodeItems objectNodeItems_;
};

#endif
