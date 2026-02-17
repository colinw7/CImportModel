#include <CQCamera3DTextureList.h>
#include <CQCamera3DCanvas.h>
#include <CQCamera3DTexture.h>
#include <CQCamera3DApp.h>

#include <CQGLTexture.h>
#include <CQUtil.h>
#include <CGeomScene3D.h>
#include <CGeomTexture.h>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QContextMenuEvent>

class CQCamera3DTextureTable : public QTableWidget {
 public:
  CQCamera3DTextureTable() { }

  QTableWidgetItem *itemFromIndex(const QModelIndex &ind) {
    return QTableWidget::itemFromIndex(ind);
  }
};

//---

CQCamera3DTextureList::
CQCamera3DTextureList(CQCamera3DCanvas *canvas) :
 QFrame(nullptr), canvas_(canvas)
{
  setObjectName("textureList");

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

  delegate_ = new CQCamera3DTextureListDelegate(this);

  table_->setItemDelegate(delegate_);

  header_ = new CQCamera3DTextureListHeader(this);

  table_->setHorizontalHeader(header_);

  header_->setSectionsClickable(true);
  header_->setStretchLastSection(true);

  //---

  connectSlots(true);
}

void
CQCamera3DTextureList::
needsUpdateSlot()
{
  valid_ = false;

  updateTextures();
}

void
CQCamera3DTextureList::
updateTextures()
{
  if (valid_)
    return;

  valid_ = true;

  //---

  connectSlots(false);

  //---

  auto *scene = canvas_->app()->getScene();

  //---

  table_->clear();

  int nt = scene->textures().size();

  table_->setRowCount   (nt + 1);
  table_->setColumnCount(2);

  table_->setHorizontalHeaderLabels(QStringList() << "Name" << "Flip");

  int r = 0;

  auto *item = new QTableWidgetItem("");

  table_->setItem(r++, 0, item);

  item->setData(Qt::UserRole, -1);

  for (auto *texture : scene->textures()) {
    auto textureName = QString::fromStdString(texture->name());

    auto *nameItem  = new QTableWidgetItem(textureName);
    auto *stateItem = new QTableWidgetItem("");

    table_->setItem(r  , 0, nameItem);
    table_->setItem(r++, 1, stateItem);

    nameItem ->setData(Qt::UserRole, texture->id());
    stateItem->setData(Qt::UserRole, texture->id());
  }

  selectedInd_ = -1;

  //---

  header_->updateColumnSizes();

  //---

  connectSlots(true);
}

void
CQCamera3DTextureList::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    canvas_->app(), SIGNAL(textureAdded()), this, SLOT(needsUpdateSlot()));
  CQUtil::connectDisconnect(b,
    canvas_->app(), SIGNAL(textureChanged()), this, SLOT(needsUpdateSlot()));

  CQUtil::connectDisconnect(b,
    table_, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),
    this, SLOT(itemSelectedSlot(QTableWidgetItem *, QTableWidgetItem *)));

  CQUtil::connectDisconnect(b, table_,
    SIGNAL(clicked(const QModelIndex &)), this, SLOT(tableClickSlot(const QModelIndex &)));
}

void
CQCamera3DTextureList::
itemSelectedSlot(QTableWidgetItem *item, QTableWidgetItem *)
{
  selectedInd_ = (item ? item->data(Qt::UserRole).toInt() : -1);

  Q_EMIT currentItemChanged();
}

void
CQCamera3DTextureList::
tableClickSlot(const QModelIndex &index)
{
  auto *texture = dynamic_cast<CQCamera3DTexture *>(textureFromIndex(index));
  if (! texture) return;

  if      (index.column() == 1)
    texture->setFlipped(! texture->isFlipped());
  else
    return;

  canvas_->app()->textureChanged();

  updateAll();
}

CGeomTexture *
CQCamera3DTextureList::
getCurrentTexture() const
{
  if (selectedInd_ < 0)
    return nullptr;

  auto *scene = canvas_->app()->getScene();

  return scene->getTextureById(selectedInd_);
}

CGeomTexture *
CQCamera3DTextureList::
getSelectedTexture() const
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

  auto *scene  = canvas_->app()->getScene();

  return scene->getTextureById(ind);
}

CGeomTexture *
CQCamera3DTextureList::
textureFromIndex(const QModelIndex &index) const
{
  auto *item = static_cast<CQCamera3DTextureTable *>(table_)->itemFromIndex(index);
  if (! item) return nullptr;

  auto ind = item->data(Qt::UserRole).toInt();
  if (ind < 0) return nullptr;

  auto *scene = canvas_->app()->getScene();

  return scene->getTextureById(ind);
}

void
CQCamera3DTextureList::
updateAll()
{
  table_->update();
  table_->viewport()->update();

  header_->viewport()->update();
  header_->update();

  canvas_->update();
}

void
CQCamera3DTextureList::
resizeEvent(QResizeEvent *)
{
  header_->updateColumnSizes();
}

QSize
CQCamera3DTextureList::
sizeHint() const
{
  QFontMetrics fm(font());

  auto s = table_->sizeHint();

  s.setHeight(s.height() + 5*fm.height());

  return s;
}

//---

CQCamera3DTextureListDelegate::
CQCamera3DTextureListDelegate(CQCamera3DTextureList *list) :
 QItemDelegate(list->table()), list_(list)
{
  setObjectName("delegate");
}

void
CQCamera3DTextureListDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *texture = dynamic_cast<CQCamera3DTexture *>(list_->textureFromIndex(index));

  if (! texture)
    return QItemDelegate::paint(painter, option, index);

  if      (index.column() == 1) {
    bool flipped = texture->isFlipped();

    Qt::CheckState checkState = (flipped ? Qt::Checked : Qt::Unchecked);

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

CQCamera3DTextureListHeader::
CQCamera3DTextureListHeader(CQCamera3DTextureList *list) :
 QHeaderView(Qt::Horizontal), list_(list)
{
  setObjectName("header");
}

void
CQCamera3DTextureListHeader::
paintEvent(QPaintEvent *e)
{
  QHeaderView::paintEvent(e);
}

void
CQCamera3DTextureListHeader::
paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
  QHeaderView::paintSection(painter, rect, logicalIndex);
}

QSize
CQCamera3DTextureListHeader::
sectionSizeFromContents(int logicalIndex) const
{
  return QHeaderView::sectionSizeFromContents(logicalIndex);
}

void
CQCamera3DTextureListHeader::
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
CQCamera3DTextureListHeader::
contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);

  menu.addAction("Fit", this, SLOT(updateColumnSizes()));

  menuSection_ = logicalIndexAt(event->pos());

  menu.exec(event->globalPos());
}
