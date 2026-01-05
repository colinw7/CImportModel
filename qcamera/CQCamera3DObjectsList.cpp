#include <CQCamera3DObjectsList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DGeomObject.h>

#include <CQUtil.h>
#include <CQTreeWidget.h>
#include <CGeomObject3D.h>

#include <QTreeWidgetItem>
#include <QVBoxLayout>

CQCamera3DObjectsList::
CQCamera3DObjectsList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("objectsList");

  auto *layout = new QVBoxLayout(this);

  tree_ = new CQTreeWidget;

  tree_->setSelectionMode(QTreeWidget::SingleSelection);

  tree_->setHeaderLabels(QStringList() << "Name");

  layout->addWidget(tree_);

  //---

  connectSlots(true);
}

void
CQCamera3DObjectsList::
updateObjects()
{
  connectSlots(false);

  //---

  auto *canvas = this->canvas_;

  tree_->clear();

  objectItem_.clear();

  auto *item = new QTreeWidgetItem(tree_, QStringList());

  tree_->addTopLevelItem(item);

  item->setData(0, Qt::UserRole, -1);

  for (auto &objectData : canvas->getObjectDatas()) {
    auto *item = createObjectItem(const_cast<CGeomObject3D *>(objectData->object()));

    item->setData(0, Qt::UserRole, objectData->ind());
  }

  //---

  connectSlots(true);
}

QTreeWidgetItem *
CQCamera3DObjectsList::
createObjectItem(CGeomObject3D *object)
{
  auto pi = objectItem_.find(object);

  if (pi != objectItem_.end())
    return (*pi).second;

  auto name = QString::fromStdString(object->getName());

  name += QString(" (%1)").arg(object->getInd());

  auto *parent = object->parent();

  QTreeWidgetItem *item;

  if (parent) {
    auto *parentItem = createObjectItem(parent);

    item = new QTreeWidgetItem(parentItem, QStringList() << name);

    parentItem->addChild(item);
  }
  else {
    item = new QTreeWidgetItem(tree_, QStringList() << name);

    tree_->addTopLevelItem(item);
  }

  objectItem_[object] = item;

  return item;
}

void
CQCamera3DObjectsList::
connectSlots(bool b)
{
  auto *canvas = this->canvas_;
  auto *app    = canvas->app();

  CQUtil::connectDisconnect(b, app, SIGNAL(modelAdded()), this, SLOT(updateObjects()));

  if (b) {
    connect(tree_, &QTreeWidget::currentItemChanged,
            this, &CQCamera3DObjectsList::objectSelectedSlot);
  }
  else {
    disconnect(tree_, &QTreeWidget::currentItemChanged,
               this, &CQCamera3DObjectsList::objectSelectedSlot);
  }
}

void
CQCamera3DObjectsList::
objectSelectedSlot(QTreeWidgetItem *item, QTreeWidgetItem *)
{
  selectedInd_ = (item ? item->data(0, Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

CGeomObject3D *
CQCamera3DObjectsList::
getSelectedIndObject() const
{
  auto *canvas = this->canvas_;

  return canvas->getObject(selectedInd_);
}

CGeomObject3D *
CQCamera3DObjectsList::
getObjectListSelected() const
{
  auto *canvas = this->canvas_;

  auto selectedItems = tree_->selectedItems();

  int ind = -1;

  for (const auto *item : selectedItems) {
    int ind1 = item->data(0, Qt::UserRole).toInt();
    if (ind1 >= 0) {
      ind = ind1;
      break;
    }
  }

  return canvas->getObject(ind);
}
