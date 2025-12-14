#include <CQNewGLObjectsList.h>
#include <CQNewGLCanvas.h>
#include <CQNewGLModel.h>
#include <CQNewGLModelObject.h>

#include <CQUtil.h>
#include <CGeomObject3D.h>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

CQNewGLObjectsList::
CQNewGLObjectsList(CQNewGLCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  auto *layout = new QVBoxLayout(this);

  tree_ = new QTreeWidget;

  tree_->setSelectionMode(QTreeWidget::SingleSelection);

  layout->addWidget(tree_);

  //---

  connectSlots(true);
}

void
CQNewGLObjectsList::
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

  for (auto &po : canvas->getObjectDatas()) {
    auto *objectData = po.second;

    auto *item = createObjectItem(objectData->object());

    item->setData(0, Qt::UserRole, objectData->ind());
  }

  //---

  connectSlots(true);
}

QTreeWidgetItem *
CQNewGLObjectsList::
createObjectItem(CGeomObject3D *object)
{
  auto pi = objectItem_.find(object);

  if (pi != objectItem_.end())
    return (*pi).second;

  auto name = QString::fromStdString(object->getName());

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
CQNewGLObjectsList::
connectSlots(bool b)
{
  auto *canvas = this->canvas_;
  auto *app    = canvas->app();

  CQUtil::connectDisconnect(b, app, SIGNAL(modelAdded()), this, SLOT(updateObjects()));

  if (b) {
    connect(tree_, &QTreeWidget::currentItemChanged,
            this, &CQNewGLObjectsList::objectSelectedSlot);
  }
  else {
    disconnect(tree_, &QTreeWidget::currentItemChanged,
               this, &CQNewGLObjectsList::objectSelectedSlot);
  }
}

void
CQNewGLObjectsList::
objectSelectedSlot(QTreeWidgetItem *item, QTreeWidgetItem *)
{
  selectedInd_ = (item ? item->data(0, Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

CGeomObject3D *
CQNewGLObjectsList::
getSelectedIndObject() const
{
  auto *canvas = this->canvas_;

  return canvas->getObject(selectedInd_);
}

CGeomObject3D *
CQNewGLObjectsList::
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
