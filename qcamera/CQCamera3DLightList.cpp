#include <CQCamera3DLightList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DLight.h>
#include <CQCamera3DApp.h>

#include <CQUtil.h>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QContextMenuEvent>

class CQCamera3DLightTable : public QTableWidget {
 public:
  CQCamera3DLightTable() { }

  QTableWidgetItem *itemFromIndex(const QModelIndex &ind) {
    return QTableWidget::itemFromIndex(ind);
  }
};

//---

CQCamera3DLightList::
CQCamera3DLightList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("lightList");

  //---

  auto *layout = new QVBoxLayout(this);

  table_ = new QTableWidget;

  table_->setObjectName("table");
  table_->setSelectionMode(QTableWidget::SingleSelection);

  table_->setSelectionMode(QAbstractItemView::SingleSelection);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);

  table_->verticalHeader()->hide();
//table_->setShowGrid(false);
  table_->setAlternatingRowColors(true);

  layout->addWidget(table_);

  //---

  delegate_ = new CQCamera3DLightListDelegate(this);

  table_->setItemDelegate(delegate_);

  header_ = new CQCamera3DLightListHeader(this);

  table_->setHorizontalHeader(header_);

  header_->setSectionsClickable(true);
  header_->setStretchLastSection(true);

  //---

  connectSlots(true);
}

void
CQCamera3DLightList::
updateLights()
{
  if (valid_)
    return;

  valid_ = true;

  //---

  connectSlots(false);

  //---

  table_->clear();

  int nl = canvas_->lights().size();

  table_->setRowCount   (nl + 1);
  table_->setColumnCount(3);

  table_->setHorizontalHeaderLabels(QStringList() << "Name" << "Enabled" << "Visible");

  int r = 0;

  auto *item = new QTableWidgetItem("");

  table_->setItem(r++, 0, item);

  item->setData(Qt::UserRole, -1);

  for (auto *light : canvas_->lights()) {
    const auto &lightName = light->name();

    auto *nameItem    = new QTableWidgetItem(QString::fromStdString(lightName));
    auto *enabledItem = new QTableWidgetItem("");
    auto *visibleItem = new QTableWidgetItem("");

    table_->setItem(r, 0, nameItem);
    table_->setItem(r, 1, enabledItem);
    table_->setItem(r, 2, visibleItem);

    nameItem   ->setData(Qt::UserRole, light->id());
    enabledItem->setData(Qt::UserRole, light->id());
    visibleItem->setData(Qt::UserRole, light->id());

    ++r;
  }

  selectedInd_ = -1;

  //---

  header_->updateColumnSizes();

  //---

  connectSlots(true);
}

void
CQCamera3DLightList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, canvas_, SIGNAL(lightAdded()), this, SLOT(invalidate()));

  CQUtil::connectDisconnect(b,
    table_, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),
    this, SLOT(itemSelectedSlot(QTableWidgetItem *, QTableWidgetItem *)));

  CQUtil::connectDisconnect(b, table_,
    SIGNAL(clicked(const QModelIndex &)), this, SLOT(tableClickSlot(const QModelIndex &)));
}

void
CQCamera3DLightList::
itemSelectedSlot(QTableWidgetItem *item, QTableWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);

  Q_EMIT currentItemChanged();
}

void
CQCamera3DLightList::
tableClickSlot(const QModelIndex &index)
{
  auto *light = lightFromIndex(index);
  if (! light) return;

  if      (index.column() == 1)
    light->setEnabled(! light->getEnabled());
  else if (index.column() == 2)
    light->setVisible(! light->isVisible());
  else
    return;

  updateAll(true);
}

CQCamera3DLight *
CQCamera3DLightList::
getCurrentLight() const
{
  if (selectedInd_ < 0)
    return nullptr;

  return canvas_->getLightById(selectedInd_);
}

CQCamera3DLight *
CQCamera3DLightList::
getSelectedLight() const
{
  auto selectedItems = table_->selectedItems();

  int ind = -1;

  for (const auto *item : selectedItems) {
    int ind1 = item->data(Qt::UserRole).toInt();
    if (ind1 >= 0) {
      ind = ind1;
      break;
    }
  }

  if (ind < 0)
    return nullptr;

  return canvas_->getLightById(ind);
}

CQCamera3DLight *
CQCamera3DLightList::
lightFromIndex(const QModelIndex &index) const
{
  auto *item = static_cast<CQCamera3DLightTable *>(table_)->itemFromIndex(index);
  if (! item) return nullptr;

  auto ind = item->data(Qt::UserRole).toInt();
  if (ind < 0) return nullptr;

  return canvas_->getLightById(ind);
}

void
CQCamera3DLightList::
updateAll(bool geom)
{
  table_->update();
  table_->viewport()->update();

  header_->viewport()->update();
  header_->update();

  if (geom)
    canvas_->updateObjectsData();

  canvas_->update();
}

void
CQCamera3DLightList::
resizeEvent(QResizeEvent *)
{
  header_->updateColumnSizes();
}

QSize
CQCamera3DLightList::
sizeHint() const
{
  QFontMetrics fm(font());

  auto s = table_->sizeHint();

  s.setHeight(s.height() + 5*fm.height());

  return s;
}

//---

CQCamera3DLightListDelegate::
CQCamera3DLightListDelegate(CQCamera3DLightList *list) :
 QItemDelegate(list->table()), list_(list)
{
  setObjectName("delegate");
}

void
CQCamera3DLightListDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *light = list_->lightFromIndex(index);

  if (! light)
    return QItemDelegate::paint(painter, option, index);

  if      (index.column() == 1 || index.column() == 2) {
    bool checked = false;

    if      (index.column() == 1)
      checked = light->getEnabled();
    else
      checked = light->isVisible();

    Qt::CheckState checkState = (checked ? Qt::Checked : Qt::Unchecked);

    QFontMetrics fm(list_->font());

    int is = fm.height();

    int x = option.rect.left() + (option.rect.width () - is)/2;
    int y = option.rect.top () + (option.rect.height() - is)/2;

    auto rect = QRect(x, y, is, is);

    QItemDelegate::drawCheck(painter, option, rect, checkState);
  }
  else
    QItemDelegate::paint(painter, option, index);
}

//---

CQCamera3DLightListHeader::
CQCamera3DLightListHeader(CQCamera3DLightList *list) :
 QHeaderView(Qt::Horizontal), list_(list)
{
  setObjectName("header");
}

void
CQCamera3DLightListHeader::
paintEvent(QPaintEvent *e)
{
  QHeaderView::paintEvent(e);
}

void
CQCamera3DLightListHeader::
paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
  QHeaderView::paintSection(painter, rect, logicalIndex);
}

QSize
CQCamera3DLightListHeader::
sectionSizeFromContents(int logicalIndex) const
{
  return QHeaderView::sectionSizeFromContents(logicalIndex);
}

void
CQCamera3DLightListHeader::
updateColumnSizes()
{
  QFontMetrics fm(list_->font());

  auto tw1 = fm.horizontalAdvance("Enabled") + 8;
  auto tw2 = fm.horizontalAdvance("Visible") + 8;

  auto *vbar = list_->table()->verticalHeader();

  int w = width() - tw1 - tw2 - vbar->width() - 8;

  resizeSection(2, tw2);
  resizeSection(1, tw1);
  resizeSection(0, w  );
}

void
CQCamera3DLightListHeader::
contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);

  menu.addAction("Fit", this, SLOT(updateColumnSizes()));

  menuSection_ = logicalIndexAt(event->pos());

  menu.exec(event->globalPos());
}
