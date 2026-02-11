#include <CQCamera3DCameraList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DCamera.h>
#include <CQCamera3DApp.h>

#include <CQUtil.h>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QContextMenuEvent>

class CQCamera3DCameraTable : public QTableWidget {
 public:
  CQCamera3DCameraTable() { }

  QTableWidgetItem *itemFromIndex(const QModelIndex &ind) {
    return QTableWidget::itemFromIndex(ind);
  }
};

//---

CQCamera3DCameraList::
CQCamera3DCameraList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("cameraList");

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

  delegate_ = new CQCamera3DCameraListDelegate(this);

  table_->setItemDelegate(delegate_);

  header_ = new CQCamera3DCameraListHeader(this);

  table_->setHorizontalHeader(header_);

  header_->setSectionsClickable(true);
  header_->setStretchLastSection(true);

  //---

  connectSlots(true);
}

void
CQCamera3DCameraList::
updateCameras()
{
  if (valid_)
    return;

  valid_ = true;

  //---

  connectSlots(false);

  //---

  table_->clear();

  int nc = canvas_->cameras().size();

  table_->setRowCount   (nc + 1);
  table_->setColumnCount(2);

  table_->setHorizontalHeaderLabels(QStringList() << "Name" << "Current");

  int r = 0;

  auto *item = new QTableWidgetItem("");

  table_->setItem(r++, 0, item);

  item->setData(Qt::UserRole, -1);

  for (auto *camera : canvas_->cameras()) {
    const auto &cameraName = camera->name();

    auto *nameItem  = new QTableWidgetItem(QString::fromStdString(cameraName));
    auto *stateItem = new QTableWidgetItem("");

    table_->setItem(r  , 0, nameItem);
    table_->setItem(r++, 1, stateItem);

    nameItem ->setData(Qt::UserRole, camera->id());
    stateItem->setData(Qt::UserRole, camera->id());
  }

  selectedInd_ = -1;

  //---

  header_->updateColumnSizes();

  //---

  connectSlots(true);
}

void
CQCamera3DCameraList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, canvas_, SIGNAL(cameraAdded()), this, SLOT(invalidate()));

  CQUtil::connectDisconnect(b,
    table_, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),
    this, SLOT(itemSelectedSlot(QTableWidgetItem *, QTableWidgetItem *)));

  CQUtil::connectDisconnect(b, table_,
    SIGNAL(clicked(const QModelIndex &)), this, SLOT(tableClickSlot(const QModelIndex &)));
}

void
CQCamera3DCameraList::
itemSelectedSlot(QTableWidgetItem *item, QTableWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);

  Q_EMIT currentItemChanged();
}

void
CQCamera3DCameraList::
tableClickSlot(const QModelIndex &index)
{
  auto *camera = cameraFromIndex(index);
  if (! camera) return;

  if      (index.column() == 1)
    canvas_->setCurrentCamera(camera);
  else
    return;

  updateAll();
}

CGLCameraIFace *
CQCamera3DCameraList::
getCurrentCamera() const
{
  if (selectedInd_ < 0)
    return nullptr;

  return canvas_->getCameraById(selectedInd_);
}

CGLCameraIFace *
CQCamera3DCameraList::
getSelectedCamera() const
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

  return canvas_->getCameraById(ind);
}

CGLCameraIFace *
CQCamera3DCameraList::
cameraFromIndex(const QModelIndex &index) const
{
  auto *item = static_cast<CQCamera3DCameraTable *>(table_)->itemFromIndex(index);
  if (! item) return nullptr;

  auto ind = item->data(Qt::UserRole).toInt();
  if (ind < 0) return nullptr;

  return canvas_->getCameraById(ind);
}

void
CQCamera3DCameraList::
updateAll()
{
  table_->update();
  table_->viewport()->update();

  header_->viewport()->update();
  header_->update();

  canvas_->update();
}

void
CQCamera3DCameraList::
resizeEvent(QResizeEvent *)
{
  header_->updateColumnSizes();
}

QSize
CQCamera3DCameraList::
sizeHint() const
{
  QFontMetrics fm(font());

  auto s = table_->sizeHint();

  s.setHeight(s.height() + 5*fm.height());

  return s;
}

//---

CQCamera3DCameraListDelegate::
CQCamera3DCameraListDelegate(CQCamera3DCameraList *list) :
 QItemDelegate(list->table()), list_(list)
{
  setObjectName("delegate");
}

void
CQCamera3DCameraListDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *camera = list_->cameraFromIndex(index);

  if (! camera)
    return QItemDelegate::paint(painter, option, index);

  auto *canvas = list_->canvas();

  if      (index.column() == 1) {
    bool current = (canvas->getCurrentCamera() == camera);

    Qt::CheckState checkState = (current ? Qt::Checked : Qt::Unchecked);

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

CQCamera3DCameraListHeader::
CQCamera3DCameraListHeader(CQCamera3DCameraList *list) :
 QHeaderView(Qt::Horizontal), list_(list)
{
  setObjectName("header");
}

void
CQCamera3DCameraListHeader::
paintEvent(QPaintEvent *e)
{
  QHeaderView::paintEvent(e);
}

void
CQCamera3DCameraListHeader::
paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
  QHeaderView::paintSection(painter, rect, logicalIndex);
}

QSize
CQCamera3DCameraListHeader::
sectionSizeFromContents(int logicalIndex) const
{
  return QHeaderView::sectionSizeFromContents(logicalIndex);
}

void
CQCamera3DCameraListHeader::
updateColumnSizes()
{
  QFontMetrics fm(list_->font());

  auto tw = fm.horizontalAdvance("Flip") + 8;

  auto *vbar = list_->table()->verticalHeader();

  int w = width() - tw - vbar->width() - 8;

  resizeSection(1, tw);
  resizeSection(0, w);
}

void
CQCamera3DCameraListHeader::
contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);

  menu.addAction("Fit", this, SLOT(updateColumnSizes()));

  menuSection_ = logicalIndexAt(event->pos());

  menu.exec(event->globalPos());
}
