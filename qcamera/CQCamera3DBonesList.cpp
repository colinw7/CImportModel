#include <CQCamera3DBonesList.h>
#include <CQCamera3DControl.h>
#include <CQCamera3DApp.h>

#include <CQUtil.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>
#include <CGeomNodeData.h>

#include <QTreeWidget>
#include <QVBoxLayout>
#include <set>

CQCamera3DBonesList::
CQCamera3DBonesList(CQCamera3DApp *app) :
 app_(app)
{
  setObjectName("bonesList");

  auto *layout = new QVBoxLayout(this);

  //--

  tree_ = new QTreeWidget;

  tree_->setSelectionMode(QTreeWidget::SingleSelection);

  layout->addWidget(tree_);

  //---

  connectSlots(true);

  updateWidgets();
}

void
CQCamera3DBonesList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, tree_,
    SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
    this, SLOT(currentItemSlot(QTreeWidgetItem *, QTreeWidgetItem *)));
}

void
CQCamera3DBonesList::
updateWidgets()
{
  connectSlots(false);

  //---

  tree_->clear();

  auto rootObjects = app_->getRootObjects();

  for (auto *object : rootObjects) {
    nodeItems_.clear();

    const auto &nodeIds = object->getNodeIds();

    for (const auto &nodeId : nodeIds) {
      createNodeItem(object, nodeId);
    }
  }

  //---

  connectSlots(true);
}

QTreeWidgetItem *
CQCamera3DBonesList::
createNodeItem(CGeomObject3D *object, int nodeId)
{
  auto pn = nodeItems_.find(nodeId);

  if (pn != nodeItems_.end())
    return (*pn).second;

  //---

  const auto &nodeData = object->getNode(nodeId);
  if (! nodeData.isValid()) return nullptr;

  auto objectName = QString(" %1").arg(QString::fromStdString(nodeData.name()));

  objectName += QString(" (%1)").arg(nodeId);

#if 0
  if (nodeData.parent() >= 0)
    objectName += QString(" Parent:%1").arg(nodeData.parent());

  if (! nodeData.children.empty()) {
    objectName += " Children:";

    int ic = 0;

    for (const auto &c : nodeData.children) {
      if (ic != 0)
        objectName += ", ";

      objectName += QString("%1").arg(c);

      ++ic;
    }
  }
#endif

  QTreeWidgetItem *item { nullptr };

  if (nodeData.parent() >= 0) {
    auto *parentItem = createNodeItem(object, nodeData.parent());

    item = new QTreeWidgetItem(parentItem, QStringList() << objectName);

    parentItem->addChild(item);
  }
  else {
    item = new QTreeWidgetItem(tree_, QStringList() << objectName);

    tree_->addTopLevelItem(item);
  }

  item->setData(0, Qt::UserRole, nodeId);

  nodeItems_[nodeId] = item;

  return item;
}

void
CQCamera3DBonesList::
currentItemSlot(QTreeWidgetItem *item, QTreeWidgetItem *)
{
  boneInd_ = item->data(0, Qt::UserRole).toInt();

#if 0
  auto rootObjects = app_->getRootObjects();

  for (auto *object : rootObjects) {
    BoneData boneData;
    getBoneData(object, boneInd_, boneData);

    std::cerr << "Node: " << boneData.name << "(" << boneData.ind << ")\n";

    for (const auto &v : boneData.vertices)
      std::cerr << "  Vertex: " << v.x << " " << v.y << " " << v.z << "\n";
  }
#endif

  Q_EMIT currentItemChanged();
}

CGeomNodeData *
CQCamera3DBonesList::
currentBoneNode() const
{
  return getBoneNode(boneInd_);
}

CGeomNodeData *
CQCamera3DBonesList::
getBoneNode(int boneInd) const
{
  if (boneInd < 0)
    return nullptr;

  auto rootObjects = app_->getRootObjects();

  for (auto *object : rootObjects) {
    const auto &node = object->getNode(boneInd);
    if (! node.isValid()) continue;

    return const_cast<CGeomNodeData *>(&node);
  }

  return nullptr;
}

//---

void
CQCamera3DBonesList::
getBoneData(CGeomObject3D *object, int nodeId, BoneData &boneData) const
{
  int boneInd = nodeId;

  const auto &nodeData = object->getNode(boneInd);

  boneData.name = nodeData.name();
  boneData.ind  = nodeData.ind();

  getNodeVertices(object, nodeId, boneData);
}

void
CQCamera3DBonesList::
getNodeVertices(CGeomObject3D *object, int nodeId, BoneData &boneData) const
{
  std::set<uint> vertexInds;

  const auto &faces = object->getFaces();

  for (const auto *face : faces) {
    const auto &vertices = face->getVertices();

    for (const auto &v : vertices) {
      const auto &vertex = object->getVertex(v);

      if (vertexInds.find(vertex.getInd()) != vertexInds.end())
        continue;

      vertexInds.insert(vertex.getInd());

      //---

      const auto &jointData = vertex.getJointData();

      bool match = false;

      for (int i = 0; i < 4; ++i) {
        if (jointData.nodeDatas[i].node == nodeId) {
          match = true;
          break;
        }
      }

      if (! match)
        continue;

      const auto &model = vertex.getModel();

      boneData.vertices.push_back(model);
    }
  }

  for (auto *child : object->children())
    getNodeVertices(child, nodeId, boneData);
}
