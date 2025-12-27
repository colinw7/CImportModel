#include <CQNewGLBonesList.h>
#include <CQNewGLControl.h>
#include <CQNewGLCanvas.h>

#include <CQUtil.h>
#include <CGeomObject3D.h>
#include <CGeomNodeData.h>

#include <QTreeWidget>
#include <QVBoxLayout>

CQNewGLBonesList::
CQNewGLBonesList(CQNewGLControl *control) :
 control_(control)
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
CQNewGLBonesList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, tree_,
    SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
    this, SLOT(currentItemSlot(QTreeWidgetItem *, QTreeWidgetItem *)));
}

void
CQNewGLBonesList::
updateWidgets()
{
  connectSlots(false);

  //---

  auto *canvas = control_->canvas();

#if 0
//auto *object = getBonesObject();
  auto *object = control_->getRootObject();
#endif

  tree_->clear();

  auto rootObjects = canvas->getRootObjects();

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
CQNewGLBonesList::
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
  if (nodeData.parent >= 0)
    objectName += QString(" Parent:%1").arg(nodeData.parent);

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
CQNewGLBonesList::
currentItemSlot(QTreeWidgetItem *item, QTreeWidgetItem *)
{
  boneInd_ = item->data(0, Qt::UserRole).toInt();

  auto *canvas = control_->canvas();

  auto rootObjects = canvas->getRootObjects();

  for (auto *object : rootObjects) {
    CQNewGLCanvas::BoneData boneData;

    canvas->getBoneData(object, boneInd_, boneData);

    std::cerr << "Node: " << boneData.name << "(" << boneData.ind << ")\n";

    for (const auto &v : boneData.vertices)
      std::cerr << "  Vertex: " << v.x << " " << v.y << " " << v.z << "\n";
  }

  canvas->setCurrentBone(boneInd_);

  if (canvas->isShowBoneVertices()) {
    canvas->updateObjectsData();
    canvas->update();
  }

  Q_EMIT currentItemChanged();
}

CGeomNodeData *
CQNewGLBonesList::
getBoneNode(int boneInd) const
{
  if (boneInd < 0)
    return nullptr;

  auto *canvas = control_->canvas();

  auto rootObjects = canvas->getRootObjects();

#if 0
//auto *object = getBonesObject();
  auto *object = control_->getRootObject();
#endif

  for (auto *object : rootObjects) {
    const auto &node = object->getNode(boneInd);
    if (! node.isValid()) continue;

    return const_cast<CGeomNodeData *>(&node);
  }

  return nullptr;
}
