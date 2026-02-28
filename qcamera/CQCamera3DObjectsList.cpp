#include <CQCamera3DObjectsList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DApp.h>
#include <CQCamera3DGeomObject.h>

#include <CQUtil.h>
#include <CQTreeWidget.h>
#include <CQPixmapCache.h>
#include <CGeomScene3D.h>
#include <CGeomObject3D.h>

#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QContextMenuEvent>

CQCamera3DObjectsList::
CQCamera3DObjectsList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("objectsList");

  //---

  auto *layout = new QVBoxLayout(this);

  tree_ = new CQTreeWidget;

  tree_->setObjectName("tree");
  tree_->setSelectionMode(QTreeWidget::SingleSelection);

  tree_->setHeaderLabels(QStringList() << "Name" << "Selected" << "Visible");

  layout->addWidget(tree_);

  //---

  selectedIcon_   = CQPixmapCacheInst->getPixmap("SELECTED");
  deselectedIcon_ = CQPixmapCacheInst->getPixmap("DESELECTED");

  visibleIcon_   = CQPixmapCacheInst->getPixmap("VISIBLE");
  invisibleIcon_ = CQPixmapCacheInst->getPixmap("INVISIBLE");

  //---

  delegate_ = new CQCamera3DObjectsListDelegate(this);

  tree_->setItemDelegate(delegate_);

  header_ = new CQCamera3DObjectsListHeader(this);

  tree_->setHeader(header_);

  header_->setSectionsClickable(true);

  //---

  connectSlots(true);
}

void
CQCamera3DObjectsList::
resizeEvent(QResizeEvent *)
{
  header_->updateColumnSizes();
}

void
CQCamera3DObjectsList::
updateObjects()
{
  connectSlots(false);

  //---

  tree_->clear();

  objectItem_.clear();

  auto *item = new QTreeWidgetItem(tree_, QStringList());

  tree_->addTopLevelItem(item);

  item->setData(0, Qt::UserRole, -1);

  auto *scene = canvas_->app()->getScene();

  for (auto *object : scene->getObjects()) {
    auto *item = createObjectItem(object);

    item->setData(0, Qt::UserRole, object->getInd());
  }

  //---

  header_->updateColumnSizes();

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
paintEvent(QPaintEvent *e)
{
  updateState();

  QFrame::paintEvent(e);
}

void
CQCamera3DObjectsList::
updateState()
{
  anyVisible_  = false;
  anySelected_ = false;

  auto *scene = canvas_->app()->getScene();

  for (auto *object : scene->getObjects()) {
    if (object->getVisible())
      anyVisible_ = true;

    if (object->getSelected())
      anySelected_ = true;
  }
}

void
CQCamera3DObjectsList::
connectSlots(bool b)
{
  auto *app = canvas_->app();

  CQUtil::connectDisconnect(b, app, SIGNAL(modelAdded()), this, SLOT(updateObjects()));

  CQUtil::connectDisconnect(b, tree_,
    SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
    this, SLOT(itemSelectedSlot(QTreeWidgetItem *, QTreeWidgetItem *)));

  CQUtil::connectDisconnect(b, tree_,
    SIGNAL(clicked(const QModelIndex &)), this, SLOT(treeClickSlot(const QModelIndex &)));
}

void
CQCamera3DObjectsList::
itemSelectedSlot(QTreeWidgetItem *item, QTreeWidgetItem *)
{
  selectedInd_ = (item ? item->data(0, Qt::UserRole).toInt() : -1);
//std::cerr << selectedInd_ << "\n";

  Q_EMIT currentItemChanged();
}

void
CQCamera3DObjectsList::
treeClickSlot(const QModelIndex &index)
{
  auto *object = objectFromIndex(index);
  if (! object) return;

  if      (index.column() == 1) {
    if (! object->getSelected())
      canvas_->selectObject(object, /*clear*/false, /*update*/true);
    else
      canvas_->deselectObject(object, /*update*/true);
  }
  else if (index.column() == 2)
    object->setVisible(! object->getVisible());
  else
    return;

  updateAll();
}

void
CQCamera3DObjectsList::
updateAll()
{
  tree_->update();
  tree_->viewport()->update();

  header_->viewport()->update();
  header_->update();

  canvas_->update();
}

CGeomObject3D *
CQCamera3DObjectsList::
getSelectedIndObject() const
{
  if (selectedInd_ < 0) return nullptr;

  auto *scene = canvas_->app()->getScene();

  return scene->getObjectByInd(selectedInd_);
}

CGeomObject3D *
CQCamera3DObjectsList::
getObjectListSelected() const
{
  auto selectedItems = tree_->selectedItems();

  int ind = -1;

  for (const auto *item : selectedItems) {
    auto ind1 = item->data(0, Qt::UserRole).toInt();

    if (ind1 >= 0) {
      ind = ind1;
      break;
    }
  }

  if (ind < 0) return nullptr;

  auto *scene = canvas_->app()->getScene();

  return scene->getObjectByInd(ind);
}

CGeomObject3D *
CQCamera3DObjectsList::
objectFromIndex(const QModelIndex &index) const
{
  auto *item = tree_->itemFromIndex(index);

  auto ind = item->data(0, Qt::UserRole).toInt();
  if (ind < 0) return nullptr;

  auto *scene = canvas_->app()->getScene();

  return scene->getObjectByInd(ind);
}

void
CQCamera3DObjectsList::
selectAllObjects(bool select)
{
  if (select)
    canvas_->selectAllObjects();
  else
    canvas_->deselectAll();

  updateAll();
}

void
CQCamera3DObjectsList::
showAllObjects(bool visible)
{
  auto *scene = canvas_->app()->getScene();

  for (auto *object : scene->getObjects()) {
    object->setVisible(visible);
  }

  updateAll();
}

//---

CQCamera3DObjectsListDelegate::
CQCamera3DObjectsListDelegate(CQCamera3DObjectsList *list) :
 QItemDelegate(list->tree()), list_(list)
{
  setObjectName("delegate");
}

void
CQCamera3DObjectsListDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *object = list_->objectFromIndex(index);

  if (! object)
    return QItemDelegate::paint(painter, option, index);

  QPixmap pm;

  if      (index.column() == 1) {
    bool selected = (object && object->getSelected());

    if (selected)
      pm = list_->selectedIcon().pixmap(option.rect.size());
    else
      pm = list_->deselectedIcon().pixmap(option.rect.size());
  }
  else if (index.column() == 2) {
    bool visible = (object && object->getVisible());

    if (visible)
      pm = list_->visibleIcon().pixmap(option.rect.size());
    else
      pm = list_->invisibleIcon().pixmap(option.rect.size());
  }
  else
    QItemDelegate::paint(painter, option, index);

  if (! pm.isNull())
    painter->drawPixmap(option.rect.topLeft(), pm);
}

//---

CQCamera3DObjectsListHeader::
CQCamera3DObjectsListHeader(CQCamera3DObjectsList *list) :
 QHeaderView(Qt::Horizontal, list->tree()), list_(list)
{
  setObjectName("objectListHeader");

  QFontMetrics fm(font());
  iconWidth_ = fm.horizontalAdvance("X") + 12;

  setMinimumSectionSize(iconWidth_);

  connect(this, SIGNAL(sectionClicked(int)), this, SLOT(clickSlot(int)));

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQCamera3DObjectsListHeader::
paintEvent(QPaintEvent *e)
{
  list_->updateState();

  QHeaderView::paintEvent(e);
}

void
CQCamera3DObjectsListHeader::
paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
  if (logicalIndex == 0)
    return QHeaderView::paintSection(painter, rect, logicalIndex);

  QPixmap pm;

  if (logicalIndex == 1) {
    if (list_->anySelected())
      pm = list_->selectedIcon().pixmap(rect.size());
    else
      pm = list_->deselectedIcon().pixmap(rect.size());
  }
  else {
    if (list_->anyVisible())
      pm = list_->visibleIcon().pixmap(rect.size());
    else
      pm = list_->invisibleIcon().pixmap(rect.size());
  }

  if (! pm.isNull())
    painter->drawPixmap(rect.topLeft(), pm);
}

QSize
CQCamera3DObjectsListHeader::
sectionSizeFromContents(int logicalIndex) const
{
  auto s = QHeaderView::sectionSizeFromContents(logicalIndex);

  iconWidth_ = s.height() - 4;

  if (logicalIndex != 0)
    s.setWidth(iconWidth_);

  return s;
}

void
CQCamera3DObjectsListHeader::
updateColumnSizes()
{
  int w = width() - 2*iconWidth();

  resizeSection(1, iconWidth());
  resizeSection(2, iconWidth());
  resizeSection(0, w);
}

void
CQCamera3DObjectsListHeader::
clickSlot(int section)
{
  if      (section == 1) {
    list_->selectAllObjects(! list_->anySelected());
  }
  else if (section == 2) {
    list_->showAllObjects(! list_->anyVisible());
  }
}

void
CQCamera3DObjectsListHeader::
contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);

  menu.addAction("Fit", this, SLOT(updateColumnSizes()));

  menuSection_ = logicalIndexAt(event->pos());

  menu.exec(event->globalPos());
}
